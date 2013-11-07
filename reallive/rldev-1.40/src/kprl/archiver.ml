(*
   Kprl: SEEN.TXT archiving, encryption, and compression handling
   Copyright (C) 2006 Haeleth

   This program is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
   details.

   You should have received a copy of the GNU General Public License along with
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple
   Place - Suite 330, Boston, MA  02111-1307, USA.
*)
(*syntax camlp4o *)

open Optpp
open Printf
open Binarray
open Bytecode


(* Retrieve offset and length of a file within an archive, or the file itself *)
let get_subfile_info archive idx =
  get_int archive (idx * 8), get_int archive (idx * 8 + 4)

let get_subfile archive idx =
  match get_subfile_info archive idx with
    | _, 0 -> None
    | pos, len -> Some (sub archive pos len)


(* RealLive's SEEN.TXT doesn't have a convenient PACL identifier in its header
   like in AVG32, so we have to test files by checking that their index table
   is sane. *)
let is_archive arr =
  if Binarray.read arr 0 23 = "\000Empty RealLive archive" then true else
  let rec test_idx = function 10000 -> true | i ->
    match get_subfile_info arr i with
      | _, 0 -> test_idx (i + 1)
      | offset, len -> if offset + len > 80000 && offset + len <= dim arr && is_bytecode arr offset
                       then test_idx (i + 1)
                       else (*(Printf.eprintf "Archive test failed: %04d\n%!" i; false)*)false in
  dim arr >= 80000 && test_idx 0


(* Wrapper for actions on an existing archive *)
let process_archive require_list (process_fun : string -> Binarray.t -> ISet.t -> unit) =
  function [] -> assert false | fname :: ranges ->
    let to_process =
      let full_range = ISet.add_range 0 9999 ISet.empty in
      if ranges = [] then
        if require_list then ISet.empty else full_range
      else
        let rec lex set =
          lexer
            | eof -> set
            | ['0'-'9']+ ["-~."]
             -> let s1 = Ulexing.latin1_sub_lexeme lexbuf 0 (Ulexing.lexeme_length lexbuf - 1) in
                let s2 =
                  (lexer
                    | ['0'-'9']+ -> Ulexing.latin1_lexeme lexbuf
                    | eof | _ -> failwith "malformed range parameter") lexbuf
                in
                lex (ISet.add_range (int_of_string s1) (int_of_string s2) set) lexbuf
            | ['0'-'9']+ -> lex (ISet.add (int_of_string (Ulexing.latin1_lexeme lexbuf)) set) lexbuf
            | "," -> lex set lexbuf
            | _ -> failwith "malformed range parameter"
        in
        ISet.inter full_range (lex ISet.empty (Ulexing.from_latin1_string (String.concat "," ranges)))
    in
    if ISet.is_empty to_process
    then
      failwith "no files to process"
    else
      let arc = read_input fname in
      if is_archive arc then
        process_fun fname arc to_process
      else
        ksprintf failwith "%s is not a valid RealLive archive" (Filename.basename fname)


(* Wrapper for actions reading an existing archive *)
let process_read (action : int -> Binarray.t -> unit) =
  process_archive false
    (fun _ arc to_process ->
       for i = 0 to 9999 do
         if ISet.mem i to_process then Option.may (action i) (get_subfile arc i)
       done)


(* Wrapper for actions on potentially unarchived files *)
let maybe_archive (action : string -> Binarray.t -> unit) =
  function [] -> assert false | (first :: _) as files ->
    if not (Sys.file_exists first) then ksprintf sysError "file `%s' not found" first;
    let f = Unix.openfile first [Unix.O_RDONLY] 0o755 in
    let process =
      if is_archive (map_file f false ~-1)
      then process_read (fun i -> action (sprintf "SEEN%04d.TXT" i))
      else List.iter (fun s -> action s (read_input s))
    in
    Unix.close f;
    process files


(* Actual processing functions *)

type action = bool * (string list -> unit)

let try_extract notify arr =
  if Bytecode.uncompressed_header (read arr 0 4)
  then false, arr
  else (notify (); true, Rlcmp.decompress arr)

let disassemble =
  true,
  fun files ->
    let ic = open_in (Filename.concat (Config.prefix ()) "reallive.kfn") in
    KfnParser.parse KfnLexer.lex (Lexing.from_channel ic);
    close_in ic;
    maybe_archive
      (fun fname arr ->
        let _, oarr = try_extract (fun () -> ()) arr in
        if !App.verbose then ksprintf sysInfo "Disassembling %s" fname;
        Disassembler.disassemble fname oarr)
      files

let extract =
  true,
  maybe_archive
    (fun fname arr ->
      let oname = Filename.concat !App.outdir (Filename.basename fname ^ ".uncompressed") in
      let processed, oarr =
        try_extract (fun () -> if !App.verbose then ksprintf sysInfo "Decompressing %s to %s" fname oname) arr
      in
      if processed
      then
        let ucheader =
          match (read_file_header arr).header_version, !App.target_version with 
            | 1, (0, 0, 0, 0) -> "KP2K" 
            | _, (0, 0, 0, 0) -> "KPRL"
            | 1, (a, b, c, d) -> sprintf "RD2K%c%c%c%c" (char_of_int d) (char_of_int c) (char_of_int b) (char_of_int a)
            | _, (a, b, c, d) -> sprintf "RDRL%c%c%c%c" (char_of_int d) (char_of_int c) (char_of_int b) (char_of_int a)
        in
        write oarr 0 ucheader;
        write_file oarr oname
      else 
        ksprintf sysInfo "Ignoring %s (not compressed)" (Filename.basename fname))

let break =
  true,
  process_read
    (fun idx arr ->
      let fname = sprintf "SEEN%04d.TXT" idx in
      if !App.verbose then ksprintf sysInfo "Extracting %s" fname;
      write_file arr (Filename.concat !App.outdir (Filename.basename fname)))

let list =
  let pad c x = Format.fprintf c "%10s k" (sprintf "%.2f" (x /. 1024.)) in
  false,
  process_read
    (fun idx arr ->
      let hdr = read_full_header arr in
      if !App.names_opt then
        if hdr.dramatis_personae <> [] then
          let rec loop =
            function
              | [] -> assert false
              | x::[] -> Format.printf "%s@." (Text.sjs_to_err x)
              | x::xs -> Format.printf "%s,@ " (Text.sjs_to_err x); loop xs
          in
          Format.printf "@[<4>SEEN%04d.TXT: " idx;
          loop (List.sort String.compare hdr.dramatis_personae)
        else
          Format.printf "SEEN%04d.TXT\n" idx
      else
        let unc = float_of_int (hdr.uncompressed_size + hdr.data_offset) in
        match hdr.compressed_size with
          | Some i -> let cmp = float_of_int (i + hdr.data_offset) in
                      Format.printf "SEEN%04d.TXT: %a -> %a   (%.2f%%)\n" idx pad unc pad cmp (cmp /. unc *. 100.)
          | None -> Format.printf "SEEN%04d.TXT: %a\n" idx pad unc)

let pack =
  true,
  List.iter
    (fun fname ->
      let arr = read_input fname in
      if not (Bytecode.uncompressed_header (read arr 0 4)) then
        ksprintf sysInfo "Skipping %s: not an uncompressed bytecode file" (Filename.basename fname)
      else
        let oname =
          Filename.concat
            !App.outdir
           (if Filename.check_suffix fname ".uncompressed"
            then Filename.basename (Filename.chop_suffix fname ".uncompressed")
            else Filename.basename fname)
        in
        try
          if !App.verbose then ksprintf sysInfo "Compressing %s to %s" fname oname;
          write_file (Rlcmp.compress arr) oname
        with
          Failure e ->
            ksprintf sysInfo "Skipping %s: %s" (Filename.basename fname) e)

let output_int oc v =
  output_byte oc (v land 0xff);
  output_byte oc ((v lsr 8) land 0xff);
  output_byte oc ((v lsr 16) land 0xff);
  output_byte oc ((v lsr 24) land 0xff)

let read_and_compress fname =
  let arr = read_input fname in
  if not (is_bytecode arr 0) then (
    ksprintf sysWarning "unable to add `%s' to archive: not a bytecode file" fname;
    None
  )
  else if Bytecode.uncompressed_header (read arr 0 4) then
    Some (Rlcmp.compress arr)
  else
    Some arr

let rebuild_arc arc fname to_process =
  let rec tfn cv = let rv = sprintf "~temp%d.seen.tmp" cv in if Sys.file_exists rv then tfn (cv + 1) else rv in
  let tfn = tfn 0 in
  Sys.rename fname tfn;
  let oc = try open_out_bin fname with Sys_error _ -> Gc.full_major (); open_out_bin fname in (* work around mmap() issues on Win32 *)
  try
    seek_out oc 80000;
    let processed, _ =
      IMap.fold
        (fun i source (rv, idx) ->
          let rlen =
            match source with
              | `Keep (offset, len)
               -> Binarray.output (Binarray.sub arc offset len) oc; len
              | `File fname
               -> (match read_and_compress fname with
                    | Some arr -> Binarray.output arr oc; Binarray.dim arr
                    | None -> 0)
          in
          IMap.add i (idx, rlen) rv, idx + rlen)
        to_process
        (IMap.empty, 80000) in
    seek_out oc 0;
    for i = 0 to 9999 do
      let offset, len = try IMap.find i processed with Not_found -> 0, 0 in
      output_int oc offset;
      output_int oc len
    done;
    close_out oc;
    Sys.remove tfn
  with
    e -> (try close_out oc with _ -> ());
         (try Sys.remove fname with _ -> ());
         (try Sys.rename tfn fname with _ -> sysWarning "handling exception in Archiver.rebuild_arc: cleanup failed, data may be corrupt");
         raise e


let add =
  false,
  function
    | [] -> assert false
    | _ :: [] -> failwith "no files to process"
    | fname :: files
     -> let arc, existing =
          if Sys.file_exists fname then
            let a = read_input fname in
            if not (is_archive a) then ksprintf failwith "%s is not a valid RealLive archive" (Filename.basename fname);
            let rec loop cv acc =
              if cv = 10000 then
                acc
              else
                loop (cv + 1)
                     (match get_subfile_info a cv with
                        | 0, 0 -> acc
                        | data -> IMap.add cv (`Keep data) acc)
            in
            a, loop 0 IMap.empty
          else
            let oc = open_out_bin fname in
            output_string oc "\000Empty RealLive archive";
            close_out oc;
            Binarray.create 0, IMap.empty
        in
        let contents, do_process =
          List.fold_left
            (fun (acc, found) fname ->
              if not (Sys.file_exists fname) then
                (ksprintf sysWarning "file not found: %s" fname; acc, found)
              else
                try
                  Scanf.sscanf
                    (Filename.basename fname)
                    "%_[Ss]%_[Ee]%_[Ee]%_[Nn]%u"
                    (fun idx -> IMap.add idx (`File fname) acc),
                  true
                with
                  Scanf.Scan_failure _ ->
                    ksprintf sysWarning
                      "unable to add `%s' to archive: file name must begin `SEENxxxx', where 0 <= xxxx <= 9999" fname;
                    acc, found)
            (existing, false)
            files
        in
        if do_process then
          rebuild_arc arc fname contents
        else
          failwith "no files to process"


let remove =
  false,
  process_archive true
    (fun fname arc to_process ->
      let any_removed = ref false
      and any_remain = ref false in
      let to_keep =
        let rec loop cv acc =
          if cv = 10000 then
            acc
          else
            loop (cv + 1)
                 (match get_subfile_info arc cv with
                    | 0, 0 -> acc
                    | data when ISet.mem cv to_process -> any_removed := true; acc
                    | data -> any_remain := true; IMap.add cv (`Keep data) acc)
        in
        loop 0 IMap.empty
      in
      if not !any_removed then
        sysInfo "No files to remove."
      else if not !any_remain then (
        sysWarning "all archive contents removed";
        let oc = try open_out_bin fname with Sys_error _ -> Gc.full_major (); open_out_bin fname in (* work around mmap() issues on Win32 *)
        for i = 0 to 9999 do output_string oc "\000\000\000\000\000\000\000\000" done;
        close_out oc
      )
      else
        rebuild_arc arc fname to_keep)
