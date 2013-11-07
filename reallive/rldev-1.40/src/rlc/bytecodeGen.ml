(*
    Rlc: bytecode file generation functions
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

open Printf
open Optpp
open KeTypes

let create_reallive bytecode bytecode_length compressed_length entrypoints kidoku_table =
  let dramatis_table =
    if !App.debug_info then
      let b = Buffer.create 0 in
      DynArray.iter (fun s -> bprintf b "%s%s\000" (str_of_int (String.length s + 1)) s) Global.dramatis_personae;
      Buffer.contents b
    else ""
  in
  let metadata = if !App.metadata then Metadata.to_string !TextTransforms.outenc !KeTypes.global_version else "" in
  let file_length = compressed_length 
                  + String.length dramatis_table 
                  + String.length metadata
                  + DynArray.length kidoku_table * 4 
                  + 0x1d0 
  in
  let file = Binarray.create file_length in
  let dramatis_offset = 0x1d0 + DynArray.length kidoku_table * 4 in
  let bytecode_offset = dramatis_offset + String.length dramatis_table + String.length metadata in
  if !App.compress then  Binarray.put_int file 0x00 0x1d0 else Binarray.write file 0x00 "KPRL";
  Binarray.put_int file 0x04 10002;
  Binarray.put_int file 0x08 0x1d0; (* Offset of kidoku_table *)
  Binarray.put_int file 0x0c (DynArray.length kidoku_table);
  Binarray.put_int file 0x10 (DynArray.length kidoku_table * 4); (* table_1 size *)
  Binarray.put_int file 0x14 dramatis_offset;
  Binarray.put_int file 0x18 (if !App.debug_info then DynArray.length Global.dramatis_personae else 0);
  Binarray.put_int file 0x1c (String.length dramatis_table);
  Binarray.put_int file 0x20 bytecode_offset;
  Binarray.put_int file 0x24 bytecode_length;
  Binarray.put_int file 0x28 compressed_length;
  Binarray.put_int file 0x2c !Global.val_0x2c;       (* #Z-1 *)
  Binarray.put_int file 0x30 (!Global.val_0x2c + 3); (* #Z-2 *)
  Array.iteri (fun i v -> Binarray.put_int file (0x34 + i * 4) v) entrypoints;
  Binarray.write file 0x1c4 (String.make 12 '\000');
  DynArray.iteri (fun i v -> Binarray.put_int file (0x1d0 + i * 4) v) kidoku_table;
  if !App.debug_info then Binarray.write file dramatis_offset dramatis_table;
  if !App.metadata then Binarray.write file (dramatis_offset + String.length dramatis_table) metadata;
  file, bytecode_offset

let create_avg2000 bytecode bytecode_length _ entrypoints kidoku_table =
  let file_length = bytecode_length + DynArray.length kidoku_table * 4 + 0x1cc in
  let file = Binarray.create file_length in
  let bytecode_offset = 0x1cc + DynArray.length kidoku_table * 4 in
  if !App.compress then  Binarray.put_int file 0x00 0x1cc else Binarray.write file 0x00 "KP2K";
  Binarray.put_int file 0x04 10002;
  let tm = Unix.localtime (Unix.time ()) in
  Binarray.put_i16 file 0x08 (tm.Unix.tm_year + 1900);
  Binarray.put_i16 file 0x0a (tm.Unix.tm_mon + 1);
  Binarray.put_i16 file 0x0c tm.Unix.tm_wday;
  Binarray.put_i16 file 0x0e tm.Unix.tm_mday;
  Binarray.put_i16 file 0x10 (tm.Unix.tm_hour + 1);
  Binarray.put_i16 file 0x12 tm.Unix.tm_min;
  Binarray.put_i16 file 0x14 tm.Unix.tm_sec;
  Binarray.write file 0x16 (String.make 10 '\000'); (* Values from 0x16 to 0x1f unknown. *)
  Binarray.put_int file 0x20 (DynArray.length kidoku_table);
  Binarray.put_int file 0x24 bytecode_length;
  Binarray.put_int file 0x28 !Global.val_0x2c;       (* #Z-1 *)
  Binarray.put_int file 0x2c (!Global.val_0x2c + 5); (* #Z-2 *)
  Array.iteri (fun i v -> Binarray.put_int file (0x30 + i * 4) v) entrypoints;
  Binarray.write file 0x1c0 (String.make 12 '\000');
  DynArray.iteri (fun i v -> Binarray.put_int file (0x1cc + i * 4) v) kidoku_table;
  file, bytecode_offset


type target_spec =
  { kidoku_len: int;
    lineno_len: int;
    kidoku_to_str: int -> string;
    lineno_to_str: int -> string;
    use_LZ77: bool;
    create_file: Binarray.t -> int -> int -> int array -> int DynArray.t -> Binarray.t * int }

let reallive_spec =
  { kidoku_len = 2;
    lineno_len = 2;
    kidoku_to_str = str16_of_int;
    lineno_to_str = str16_of_int;
    use_LZ77 = true;
    create_file = create_reallive; }

let avg2000_spec =
  { kidoku_len = 4;
    lineno_len = 4;
    kidoku_to_str = str_of_int;
    lineno_to_str = str_of_int;
    use_LZ77 = false;
    create_file = create_avg2000; }

open Codegen.Output

let generate () =
  let spec =
    match !global_target with
      | `Default
      | `RealLive
      | `Kinetic -> reallive_spec
      | `Avg2000 -> avg2000_spec
      | `Compare _ -> assert false
  in
  (* Remove duplicate entrypoints, always using the last definition of each. *)
  let entrypoints = Array.make 100 ~-1 in
  for idx = 0 to DynArray.length bytecode - 1 do
    match DynArray.unsafe_get bytecode idx with
      | Entrypoint i
        -> if entrypoints.(i) != ~-1 then DynArray.unsafe_set bytecode entrypoints.(i) (Code "");
           entrypoints.(i) <- idx  
      | _ -> ()
  done;
  (* Traverse bytecode structure once, gathering label locations and the like. *)
  let labels = Hashtbl.create 0
  and entrypoints = Array.make 100 0
  and kidoku_table = DynArray.create () in
  let bytecode_length =
    DynArray.fold_left
      (fun acc ->
        function
          | Code s -> acc + String.length s
          | LabelRef _ -> acc + 4
          | Label t -> Hashtbl.add labels t acc;
                       acc
          | Kidoku i -> DynArray.add kidoku_table (if !App.debug_info then i else 0);
                        acc + 1 + spec.kidoku_len
          | Entrypoint i -> entrypoints.(i) <- acc;
                            DynArray.add kidoku_table (i + 1_000_000);
                            acc + 1 + spec.kidoku_len
          | Lineref i -> acc + 1 + spec.lineno_len)
      0
      bytecode
  in
  (* Traverse again, building actual bytecode file. *)
  let buffer = Binarray.create ((if spec.use_LZ77 then bytecode_length * 9 / 8 else bytecode_length) + 9) in
  let entrypoint_char = 
    let ct = Ini.get_int "KIDOKU_TYPE" ~def:0 in
    let ct = if ct > 0 then ct else if !global_version > (1, 2, 5, 0) then 2 else 1 in
    if ct = 2 then '!' else '@' 
  in
  let kidoku_idx = ref 0 in
  ignore
  (DynArray.fold_left
    (fun idx ->
      function
        | Code s
           -> Binarray.write buffer ~idx s;
              idx + String.length s
        | LabelRef (l, t)
           -> if not (Hashtbl.mem labels t) then ksprintf (error l) "reference to undefined label @%s" (Text.to_sjs t);
              Binarray.put_int buffer ~idx (Hashtbl.find labels t);
              idx + 4
        | Label _ -> idx
        | Kidoku _
           -> ksprintf (Binarray.write buffer ~idx) "@%s" (spec.kidoku_to_str !kidoku_idx);
              incr kidoku_idx; idx + 1 + spec.kidoku_len
        | Entrypoint i
           -> ksprintf (Binarray.write buffer ~idx) "%c%s" entrypoint_char (spec.kidoku_to_str !kidoku_idx);
              incr kidoku_idx; idx + 1 + spec.kidoku_len
        | Lineref i
           -> ksprintf (Binarray.write buffer ~idx) "\x0a%s" (spec.lineno_to_str i);
              idx + 1 + spec.lineno_len)
      8 bytecode);
  (* Compress if required. *)
  let compressed_length, bytecode =
    if !App.compress then
      if spec.use_LZ77 then (
        if !App.verbose then sysInfo "Compressing and encrypting";
        let compressed_length = Rlcmp.c_compress (Binarray.sub buffer 8 bytecode_length) + 8 in
        Binarray.put_int buffer 0 compressed_length;
        Binarray.put_int buffer 4 bytecode_length;
        let bytecode = Binarray.sub buffer 0 compressed_length in
        Rlcmp.c_apply_mask bytecode 0;
        compressed_length, bytecode
      )
      else (
        if !App.verbose then sysInfo "Encrypting";
        let bytecode = Binarray.sub buffer 8 bytecode_length in
        Rlcmp.c_apply_mask bytecode 0;
        bytecode_length, bytecode
      )
    else
      bytecode_length, Binarray.sub buffer 8 bytecode_length
  in
  (* Write output file. *)
  if !App.verbose then sysInfo "Writing output";
  let file, bytecode_offset = spec.create_file bytecode bytecode_length compressed_length entrypoints kidoku_table in
  Binarray.blit bytecode (Binarray.sub file bytecode_offset compressed_length);
  match !App.outfile with
    | "-" -> for i = 0 to Binarray.dim file - 1 do print_char (Obj.magic file.{i}) done
    | s -> let fname = if s = "" then "rlas_output" else try Filename.chop_extension s with _ -> s in
           try
             Binarray.write_file file (Filename.concat !App.outdir (fname ^ (if !App.compress then ".TXT" else ".TXT.uncompressed")))
           with Sys_error e ->
             sysError e
