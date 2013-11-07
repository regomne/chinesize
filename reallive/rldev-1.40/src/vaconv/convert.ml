(*
   vaconv: generic conversion routines
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

open App
open Optpp
open Image
open Printf

let extension s = ExtString.String.slice s ~first:(String.rindex s '.' + 1)

let detect_format fname =
  if !iformat.name <> "" then !iformat else
    (* If we can go by the filename, do so. *)
    try 
      formats#find_extension (extension fname) 
    with Not_found ->
      (* If that fails, try identifying files by their headers. *)
      let ic = open_in_bin fname in
      try 
        let rv = formats#find_by_header ic in
        close_in ic;
        rv
      with Not_found ->
        ksprintf sysInfo "Unable to determine the format of %s; skipping it" fname;
        !iformat

let convert_file fname =
  if not (Sys.file_exists fname) then
    ksprintf sysInfo "File not found: %s" fname
  else match detect_format fname with
    | { name = "" } -> () (* format unknown, skip file *)
    | fmt ->
      if !oformat.name = "" then oformat := formats#default_conversion fmt;
      let wr = ref "read" in
      if fmt.name = !oformat.name then
        ksprintf sysInfo "Skipping %s: already a %s file" fname fmt.name
      else try
        (* Read data to intermediate format. *)
        if !verbose then ksprintf sysInfo "Reading %s file %s" fmt.name fname;
        let img = fmt.of_file fname in
        (* If writing a G00, look for XML metadata; if reading a G00 and metadata output
           is disabled, strip any that we may have read. *)
        let metafile =
          if !metafile <> ""
          then !metafile
          else try Filename.chop_extension fname ^ ".xml"
               with Invalid_argument "Filename.chop_extension" -> fname ^ ".xml"
        in
        let img =
          if not !use_meta then
            { img with metadata = None }
          else if not !oformat.use_meta then
            img
          else
            if not (Sys.file_exists metafile) then (
              if !App.metafile = ""
              then (if !verbose then ksprintf sysInfo "No metadata file found")
              else ksprintf sysInfo "Unable to read metadata from %s: file not found" metafile;
              img
            )
            else
              (* We have a metadata file candidate, so parse it. *)
              let xp = XmlParser.make () in
              if !verbose then ksprintf sysInfo "Reading metadata from %s" metafile;
              XmlParser.prove xp false;
              let dtd =
                try
                  Dtd.check (Dtd.parse_file (Filename.concat (Config.prefix ()) "vas_g00.dtd"))
                with
                  | Xml.File_not_found _ -> sysError "vas_g00.dtd not found. Please install RLdev properly"
                  | Dtd.Parse_error _ -> sysError "vas_g00.dtd appears to be invalid. Please reinstall RLdev"
              in
              let xml =
                try
                  XmlParser.parse xp (XmlParser.SFile metafile)
                with
                  | Xml.File_not_found _ -> assert false
                  | Xml.Error _ -> raise (Bad_metadata (sprintf "%s is not well-formed XML" metafile))
              in
              let metadata =
                try
                  Dtd.prove dtd "vas_g00" xml
                with
                  _ -> raise (Bad_metadata (sprintf "%s is not a valid vas_g00 document" metafile))
              in
              { img with metadata = Some ("vas_g00.dtd", metadata) }
        in
        (* Write output file. *)
        wr := "write";
        let preserve_dir = !outdir = "PRESERVE" in
        let outname, metafile =
          let outname =
            if !outfile <> ""
            then !outfile
            else let ext = String.lowercase (List.hd !oformat.extensions) in
                 sprintf "%s.%s" (try Filename.chop_extension fname with _ -> fname) ext
          in
          if preserve_dir then
            outname, metafile
          else if !outdir <> "" then
            Filename.concat !outdir (Filename.basename outname),
            Filename.concat !outdir (Filename.basename metafile)
          else
            Filename.basename outname,
            Filename.basename metafile
        in
        if !verbose then sysInfo "Converting and writing output";
        let outmeta = !oformat.to_file img outname !osubfmt in
        match outmeta with
          | None -> ()
          | Some _ when not !use_meta -> if !verbose then ksprintf sysInfo "Ignoring metadata"
          | Some metadata -> if !verbose then ksprintf sysInfo "Writing metadata to %s" metafile;
                             let oc = open_out metafile in
                             output_string oc metadata;
                             close_out oc
      with
        (* Errors with files *)
        | Unsupported msg -> ksprintf sysInfo "Unable to %s %s: %s" !wr fname msg
        | Malformed -> ksprintf sysInfo "Unable to %s %s: data seems to be corrupt" !wr fname
        (* Errors with PNGs *)
        | Failure "generic png" -> ksprintf sysInfo "Unable to %s %s: PNG data error" !wr fname
        | Failure "out of memory" -> ksprintf sysInfo "Unable to %s %s: image too large to fit in memory" !wr fname
        | Failure "unreported open failure" -> ksprintf sysInfo "Unable to %s %s: error opening file" !wr fname
        (* Errors converting data *)
        | Convert_error msg -> ksprintf sysInfo "Unable to convert %s: %s" fname msg
        | Bad_metadata msg -> ksprintf sysInfo "Unable to convert %s due to bad metadata: %s" fname msg
