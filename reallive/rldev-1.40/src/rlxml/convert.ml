(*
   RlXml: general conversion logic
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

open Optpp
open Printf
open ExtString

let string_of_xml (dtd, xml) =
  sprintf
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE %s SYSTEM \"%s\">\n%s"
    (Xml.tag xml)
    dtd
    (Xml.to_string_fmt xml)

let read_xml_file dtdname filename =
  let dtdfile = dtdname ^ ".dtd" in
  let xp = XmlParser.make () in
  XmlParser.prove xp false;
  try
    let dtd = Dtd.check (Dtd.parse_file (Filename.concat (Config.prefix ()) dtdfile))
    and xml = XmlParser.parse xp (XmlParser.SFile filename) 
    in Dtd.prove dtd dtdname xml
  with
    | Xml.File_not_found missing 
       -> assert (Filename.check_suffix missing dtdfile);
          ksprintf failwith "%s not found. Please install RLdev properly" dtdfile
    | Xml.Error e 
       -> ksprintf failwith "%s is not well-formed XML: %s" filename (Xml.error e)
    | Dtd.Prove_error e 
       -> ksprintf failwith "%s is not a valid %s document: %s" filename dtdname (Dtd.prove_error e)
    | Dtd.Check_error (Dtd.ElementNotDeclared _ as e) 
       -> ksprintf failwith "%s is not a valid %s document: %s" filename dtdname (Dtd.check_error e)
    | Dtd.Check_error _ | Dtd.Parse_error _ 
       -> ksprintf failwith "%s appears to be invalid. Please reinstall RLdev" dtdfile
  
let convert ?(single = false) file =
  let pref, ext = 
    try 
      let idx = String.rindex file '.' in
      String.slice file ~last:idx, String.lowercase (String.slice file ~first:idx)
    with Not_found -> 
      file, ""
  in
  let outext, conversion =
    match ext with
      | ".gan" -> ".ganxml", `ToXml Gan.of_input
      | ".ganxml" -> ".gan", `OfXml (Gan.to_output, "vas_gan")
      | _ -> failwith "unknown file type"
  in
  let outfile = 
    if single && !App.outdir <> "" then
      if Filename.check_suffix !App.outdir outext
      then !App.outdir
      else !App.outdir ^ outext
    else
      Filename.concat !App.outdir (pref ^ outext) 
  in
  match conversion with
    | `ToXml f
       -> let ic = IO.input_channel (open_in_bin file) in
          let (dtd, _) as xml = f ic in
          if !App.verbose then ksprintf sysInfo "Generating XML in %s format" (if Filename.check_suffix dtd ".dtd" then Filename.chop_suffix dtd ".dtd" else dtd);
          let out = string_of_xml xml in
          IO.close_in ic;
          let oc = open_out outfile in
          output_string oc out;
          close_out oc
    | `OfXml (f, dtd)
       -> if !App.verbose then ksprintf sysInfo "Reading and validating XML";
          let xml = read_xml_file dtd file in
          let oc = IO.output_channel (open_out_bin outfile) in
          f oc xml;
          IO.close_out oc
