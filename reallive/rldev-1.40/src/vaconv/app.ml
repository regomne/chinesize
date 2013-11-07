(*
   vaconv: VisualArt's bitmap format converter
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
    exe_name = "vaconv";
    name = "Vaconv";
    description = "VisualArt's bitmap format converter" }

let verbose = ref false
let brutal = ref false
let outdir = ref ""
let outfile = ref ""
let oformat = ref Image.dummy_format
let iformat = ref Image.dummy_format
let osubfmt = ref ""
let metafile = ref ""
let use_meta = ref true
let debug_dump_file = ref false
let include_parts = ref false
let masked_only = ref false
