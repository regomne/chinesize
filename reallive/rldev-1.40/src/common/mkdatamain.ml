(*
   RLdev: program to generate marshalled data tables
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

let tables = Cp932_in.sb_to_uni, Cp932_in.db_to_uni, Cp932_in.uni_to_sb, Cp932_in.uni_to_db in
let oc = open_out_bin (Filename.concat Sys.argv.(1) "cp932.dat") in
Marshal.to_channel oc tables [];
close_out oc;

let tables = Cp936_in.db_to_uni, Cp936_in.uni_to_db in
let oc = open_out_bin (Filename.concat Sys.argv.(1) "cp936.dat") in
Marshal.to_channel oc tables [];
close_out oc;

let tables = Cp949_in.db_to_uni, Cp949_in.uni_to_db, Cp949_in.map_encode, Cp949_in.map_decode in
let oc = open_out_bin (Filename.concat Sys.argv.(1) "cp949.dat") in
Marshal.to_channel oc tables [];
close_out oc;
