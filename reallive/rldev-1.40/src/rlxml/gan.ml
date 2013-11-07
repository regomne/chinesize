(*
   RlXml: GAN format handling
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
open ExtList
open Xml


(* Frame record type *)

type 'a triopt = [ `None | `Some of 'a | `Invalid ]

type frame =
  { patt: int32 triopt;  
       x: int32 triopt;
       y: int32 triopt;
    time: int32 triopt;
   alpha: int32 triopt;
   other: int32 triopt; }

let new_frame =
  { patt = `None;
       x = `None;
       y = `None;
    time = `None;
   alpha = `None;
   other = `None; }


(* Convert a frame record into an attribute list *)

let get_frame attrs =
  List.filter_map
    (function
      | n, `Some v -> Some (n, Int32.to_string v)
      | _ -> None)
    ["pattern", attrs.patt;
           "x", attrs.x;
           "y", attrs.y;
        "time", attrs.time;
       "alpha", attrs.alpha;
       "other", attrs.other;]


(* Convert a frame record into a <frame> element *)

let get_frame_elt defattrs attrs =
  let attrs' =
    { patt = if defattrs.patt  <> `Invalid then `None else attrs.patt;
         x = if defattrs.x     <> `Invalid then `None else attrs.x;
         y = if defattrs.y     <> `Invalid then `None else attrs.y;
      time = if defattrs.time  <> `Invalid then `None else attrs.time;
     alpha = if defattrs.alpha <> `Invalid then `None else attrs.alpha;
     other = if defattrs.other <> `Invalid then `None else attrs.other; }
  in
  Element ("frame", get_frame attrs', [])


(* Build a frame record containing all attributes that are constant throughout a set *)

let get_default_attrs =
  function
    | [] -> invalid_arg "get_default_attrs"
    | hd :: tl
       -> List.fold_left
            (fun a b ->
               { patt = if a.patt  <> b.patt  then `Invalid else a.patt;
                    x = if a.x     <> b.x     then `Invalid else a.x;
                    y = if a.y     <> b.y     then `Invalid else a.y;
                 time = if a.time  <> b.time  then `Invalid else a.time;
                alpha = if a.alpha <> b.alpha then `Invalid else a.alpha; 
                other = if a.other <> b.other then `Invalid else a.other; })
            hd tl


(* Read a single frame *)

let rec read_frame ic attrs =
  let tag = IO.read_i32 ic in
  if tag = 999999 then
    attrs
  else
    let value = IO.read_real_i32 ic in
    let attrs' =
      match tag with
        | 30_100 -> { attrs with  patt = `Some value }
        | 30_101 -> { attrs with     x = `Some value }
        | 30_102 -> { attrs with     y = `Some value }
        | 30_103 -> { attrs with  time = `Some value }
        | 30_104 -> { attrs with alpha = `Some value }
        | 30_105 -> { attrs with other = `Some value }
        | _ -> ksprintf failwith "unknown GAN frame tag %d" tag
    in
    read_frame ic attrs'


(* Read all frames in a set *)

let rec read_set_frames ic frames frames_left =
  if frames_left != 0 then
    let frame = read_frame ic new_frame in
    read_set_frames ic (frame :: frames) (frames_left - 1)
  else
    let defattrs = get_default_attrs frames in
    let frames = List.rev_map (get_frame_elt defattrs) frames in
    Element ("set", get_frame defattrs, frames)
    
    
(* Read all sets in a file *)

let read_sets ic =
  let rec loop sets count =
    if count = 0 then
      List.rev sets
    else
      let this =
        (* Main set-reading logic: check set header, then call read_frames. *)
        if IO.read_i32 ic <> 30_000 then failwith "expected start of GAN set";
        let frame_count = IO.read_i32 ic in
        if frame_count = 0 then failwith "expected animation to contain at least one frame";
        read_set_frames ic [] frame_count
      in
      loop (this :: sets) (count - 1)
  in
  loop []


(* Read a file *)

let of_input ic =
  if !App.verbose then ksprintf sysInfo "Reading GAN header";
  (* Read header *)
  let a = IO.read_i32 ic in
  let b = IO.read_i32 ic in
  let c = IO.read_i32 ic in
  if a != 10_000 || b != 10_000 || c != 10_100 then failwith "unknown value in GAN header";
  (* Read filename *)
  let len = IO.read_i32 ic in
  let g00 = IO.really_nread ic (len - 1) in
  if IO.read ic != '\000' then failwith "incorrect filename length in GAN header";
  (* Read data header *)
  if IO.read_i32 ic != 20_000 then failwith "expected start of GAN data section";
  let sets = IO.read_i32 ic in
  (* Read data *)
  if !App.verbose then ksprintf sysInfo "Reading GAN set data";
  let set_list = read_sets ic sets in
  (* Return XML *)
  "vas_gan.dtd", Element ("vas_gan", ["bitmap", g00], set_list)


(* Write a single frame record *)

let write_frame oc attrs =
  Array.iter
    (fun (elt, tag) ->
      try 
        let value = List.assoc elt attrs in
        IO.write_i32 oc tag;
        IO.write_real_i32 oc (Int32.of_string value)
      with
        | Not_found -> ()
        | Failure "int_of_string" -> failwith "all frame/set attributes must be integers")
    [| "PATTERN", 30_100;
             "X", 30_101;
             "Y", 30_102;
          "TIME", 30_103;
         "ALPHA", 30_104;
         "OTHER", 30_105; |];
  IO.write_i32 oc 999999        

(* Write a file *)
  
let write_gan oc g00 set_list =
  if !App.verbose then ksprintf sysInfo "Writing GAN header";
  (* Write header *)
  IO.write_i32 oc 10_000;
  IO.write_i32 oc 10_000;
  IO.write_i32 oc 10_100;
  (* Write filename *)
  IO.write_i32 oc (String.length g00 + 1);
  IO.write_string oc g00;
  (* Write data header *)
  IO.write_i32 oc 20_000;
  IO.write_i32 oc (List.length set_list);
  (* Write data *)
  if !App.verbose then ksprintf sysInfo "Writing GAN set data";
  List.iter
    (function
      | Element ("set", defattrs, frames) 
         -> IO.write_i32 oc 30_000;
            IO.write_i32 oc (List.length frames);
            List.iter 
              (function
                | Element ("frame", attrs, []) -> write_frame oc (List.rev_append attrs defattrs)
                | _ -> failwith "expected <frame>")
              frames
      | _ -> failwith "expected <set>")
    set_list

let to_output oc =
  function
    | Element ("vas_gan", ["BITMAP", g00], set_list) -> write_gan oc g00 set_list
    | _ -> failwith "expected <vas_gan>"