(*
   Kprl: RealLive archiver and disassembler
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

let app =
  { Config.app_info with
    exe_name = "kprl";
    name = "Kprl";
    description = "archiver/disassembler for RealLive";
    usage =
      "<options> <files or ranges>\n\
       \n\
       When adding to an archive, the first filename must be the archive name and \
       the rest must be names of files to add. When disassembling or decompressing, \
       if the first filename is NOT an archive, all files are taken as names of \
       separate bytecode files. In all other cases, the first filename is the name \
       of an archive and the rest should be a list of numerical ranges of files to \
       be processed (e.g. `50 60 100-150'); if omitted, all files in the archive \
       will be processed." }

let verbose = ref false
let outdir = ref ""
let names_opt = ref false
let enc = ref Config.default_encoding
let bom = ref false

let auto_target = ref true
and target_version = ref (0, 0, 0, 0)
and target_interpreter = ref ""
and force_meta : [`Chinese | `None | `Western | `Korean ] option ref = ref None
