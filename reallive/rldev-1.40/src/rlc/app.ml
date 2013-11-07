(*
   Rlc: RealLive-compatible compiler
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
    exe_name = "rlc";
    name = "Rlc";
    usage = "<options> file";
    description = "RealLive-compatible compiler" }

and verbose  = ref false
and compress = ref true
and outdir   = ref ""
and outfile  = ref ""
and gameexe  = ref ""
and enc      = ref Config.default_encoding
and old_vars = ref false
and with_rtl = ref true
and assertions = ref true
and debug_info = ref true
and metadata = ref true
and array_bounds = ref false
and flag_labels = ref false
and opt_level = ref 1l