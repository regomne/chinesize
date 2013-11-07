(*
   vaconv: G00 bitmap format
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

open Bigarray
open Image
open ExtList

(*
  Format 0 - basic RGB bitmap

    Header:
      char identifier = 0
      short width
      short height
      long compressed_size
      long uncompressed_size
      char data[compressed_size - 8]

    Data:
      struct { char r, g, b } pixels[width, height]
*)

external decompress_0 : Binarray.t -> rgb -> unit = "va_decompress_g00_0"
external compress_0 : rgb -> bool -> int = "va_compress_g00_0"

let decode_format_0 arr width height =
  (* Read header *)
  let compressed_size = Binarray.get_int arr 0x05
  and uncompressed_size = Binarray.get_int arr 0x09 in
  if uncompressed_size != width * height * 4 then raise Malformed;
  if compressed_size != Binarray.dim arr - 5 then raise Malformed;
  (* Read data *)
  let bytes = Binarray.sub arr 0x0d (compressed_size - 8) in
  let rgb = new_rgb width height in
  decompress_0 bytes rgb;
  (* Return internal representation *)
  { width = width;
    height = height;
    data = `RGB rgb;
    metadata = None }


let encode_format_0 img =
  let rgb, a = get_separate img in
  if a <> None then Optpp.sysWarning "forced format 0 encoding for RGBA image: discarding mask data";
  let compressed_size = compress_0 rgb !App.brutal in
  let arr = Binarray.create (compressed_size + 0x0d) in
  arr.{0} <- 0;
  Binarray.put_i16 arr 0x01 img.width;
  Binarray.put_i16 arr 0x03 img.height;
  Binarray.put_int arr 0x05 (compressed_size + 8);
  Binarray.put_int arr 0x09 (img.width * img.height * 4);
  Binarray.blit (Binarray.sub (linear_rgb rgb) 0 compressed_size) (Binarray.sub arr 0x0d compressed_size);
  arr, None


(*
  Format 1 - basic paletted bitmap

    Header:
      char identifier = 1
      short width
      short height
      long compressed_size
      long uncompressed_size
      char data[compressed_size - 8]

    Data:
      short palette_len
      struct { char r, g, b, a } palette[palette_len]
      char pixels[width, height]
*)

external decompress_1 : Binarray.t -> Binarray.t -> unit = "va_decompress_g00_1"
external compress_1 : Binarray.t -> bool -> int = "va_compress_g00_1"

let decode_format_1 arr width height =
  (* Read header *)
  let compressed_size = Binarray.get_int arr 0x05
  and uncompressed_size = Binarray.get_int arr 0x09 in
  if compressed_size != Binarray.dim arr - 5 then raise Malformed;
  (* Decompress data *)
  let data = Binarray.create uncompressed_size in
  decompress_1 (Binarray.sub arr 0x0d (compressed_size - 8)) data;
  (* Read data header *)
  let palette_len = Binarray.get_i16 data 0 in
  let palette = reshape_2
    (genarray_of_array1 (Binarray.sub data 2 (palette_len * 4)))
    palette_len 4
  and bytes = reshape_2
    (genarray_of_array1 (Binarray.sub data (palette_len * 4 + 2) (width * height)))
    height width
  and rgba = new_rgba width height
  in
  (* Expand data *)
  for j = 0 to width - 1 do
    for i = 0 to height - 1 do
      let idx = bytes.{i, j} in
      rgba.{i, j, 0} <- palette.{idx, 0};
      rgba.{i, j, 1} <- palette.{idx, 1};
      rgba.{i, j, 2} <- palette.{idx, 2};
      rgba.{i, j, 3} <- palette.{idx, 3}
    done
  done;
  (* Return internal representation *)
  { width = width;
    height = height;
    data = `RGBA rgba;
    metadata = None }


let encode_format_1 img colours rgba =
  assert (Array3.dim3 rgba = 4);
  let palette_map, palette_len = RGBASet.fold (fun c (m, i) -> RGBAMap.add c i m, i + 1) colours (RGBAMap.empty, 0) in
  assert (palette_len > 0 && palette_len <= 256);
  let uncompressed_size = 2 + palette_len * 4 + img.width * img.height in
  let uncompressed = Binarray.create uncompressed_size in
  Binarray.put_i16 uncompressed 0 palette_len;
  let palette = reshape_2
    (genarray_of_array1 (Binarray.sub uncompressed 2 (palette_len * 4)))
    palette_len 4
  in
  RGBAMap.iter
    (fun (a, b, c, d) idx ->
      palette.{idx, 0} <- a;
      palette.{idx, 1} <- b;
      palette.{idx, 2} <- c;
      palette.{idx, 3} <- d)
    palette_map;
  let bytes = reshape_2
    (genarray_of_array1 (Binarray.sub uncompressed (palette_len * 4 + 2) (img.width * img.height)))
    img.height img.width
  in
  for j = 0 to img.width - 1 do
    for i = 0 to img.height - 1 do
      bytes.{i, j} <- RGBAMap.find (rgba.{i, j, 0}, rgba.{i, j, 1}, rgba.{i, j, 2}, rgba.{i, j, 3}) palette_map
    done
  done;
  let compressed_size = compress_1 uncompressed !App.brutal in
  let arr = Binarray.create (compressed_size + 0x0d) in
  arr.{0} <- 1;
  Binarray.put_i16 arr 0x01 img.width;
  Binarray.put_i16 arr 0x03 img.height;
  Binarray.put_int arr 0x05 (compressed_size + 8);
  Binarray.put_int arr 0x09 uncompressed_size;
  Binarray.blit (Binarray.sub uncompressed 0 compressed_size) (Binarray.sub arr 0x0d compressed_size);
  arr, None


(*
  Format 2 - funky RGBA bitmap, with the ability to contain multiple sub-images etc.

    Header:
      char identifier = 2
      short width
      short height
      long region_count
      struct { int x1, y1, x2, y2, origin_x, origin_y } regions[region_count]
      long compressed_size
      long uncompressed_size
      char data[compressed_size - 8]

    Data:
      long block_count (always == region_count?)
      struct { int offset, length } block_index[block_count] // negative length for duplicate regions
      block_data block_data[block_count]

    Block data:
      short ??? // 1 in all cases?
      short part_count
  4   long block_x
  8   long block_y
  c   long block_width
 10   long block_height
 14   long origin_x
 18   long origin_y
 1c   long ???_width
 20   long ???_height
      [80 bytes of zeroes]
      part_data part_data[block_count]

    Part data:
      short part_x       // the part is copied to (part_x + regions[block].x1,
      short part_y       //                        part_y + regions[block].y1)
      short part_trans   // whether to use the part's mask when the G00 is used as an object?
      short part_width
      short part_height
      short ???
      [80 bytes of zeroes]
      struct { char r, g, b, a } pixels[part_width, part_height]

    Areas of the bitmap not defined in any parts appear to be initialised to
    (0, 0, 0, 0), i.e. 100% transparent black.
*)

type region =
  { x1: int; y1: int; x2: int; y2: int; ox: int; oy: int;
    mutable parts: part list; }

and part =
  { px: int; py: int; pw: int; ph: int; trans: int; }

let xml_of_regions regions =
  let elt a b c = Xml.Element (a, b, c) in
  "vas_g00.dtd",
  (elt "vas_g00" ["format", "2"]
    [elt "regions" []
      (List.map
        (fun r ->
          elt "region"
            ["x1", string_of_int r.x1;
             "y1", string_of_int r.y1;
             "x2", string_of_int r.x2;
             "y2", string_of_int r.y2]
            (let parts =
               List.map
                (fun p ->
                  elt "part"
                    ["x", string_of_int p.px;
                     "y", string_of_int p.py;
                     "w", string_of_int p.pw;
                     "h", string_of_int p.ph;
                     "trans", string_of_int p.trans]
                    [])
                r.parts
              in
              if r.ox <> 0 || r.oy <> 0 
              then (elt "origin" ["x", string_of_int r.ox; "y", string_of_int r.oy] []) :: parts
              else parts))
        regions)])


let decode_format_2 arr width height =
  (* Dump data in debug mode *)
  if !App.debug_dump_file then (
    let region_count = Binarray.get_int arr 5 in
    let h_offset = region_count * 24 + 9 in
    let compressed_size = Binarray.get_int arr h_offset
    and uncompressed_size = Binarray.get_int arr (h_offset + 4) in
    let data = Binarray.create (h_offset + uncompressed_size) in
    decompress_1 (Binarray.sub arr (h_offset + 8) (compressed_size - 8)) (Binarray.sub data h_offset uncompressed_size);
    Binarray.blit (Binarray.sub arr 0 h_offset) (Binarray.sub data 0 h_offset);
    Binarray.write_file data "debug_dump.raw"
  );
  (* Read header *)
  let region_count = Binarray.get_int arr 5 in
  let regions =
    List.init region_count
      (fun i ->
        let p = 0x09 + i * 24 in
        { x1 = Binarray.get_int arr p;
          y1 = Binarray.get_int arr (p + 4);
          x2 = Binarray.get_int arr (p + 8);
          y2 = Binarray.get_int arr (p + 12);
          ox = Binarray.get_int arr (p + 16);
          oy = Binarray.get_int arr (p + 20);
          parts = []})
  in
  let h_offset = region_count * 24 + 9 in
  let compressed_size = Binarray.get_int arr h_offset
  and uncompressed_size = Binarray.get_int arr (h_offset + 4) in
  if compressed_size != Binarray.dim arr - h_offset then raise Malformed;
  (* Decompress data; uses the same algorithm as format 1 *)
  let data = Binarray.create uncompressed_size in
  decompress_1 (Binarray.sub arr (h_offset + 8) (compressed_size - 8)) data;
  (* Read data header *)
  let index_len = Binarray.get_int data 0 in
  if index_len <> region_count then raise Malformed;
  let index = (* (offset, length) array; negative length ?= same as a previous entry *)
    List.init index_len
      (fun i ->
        Binarray.get_int data (i * 8 + 4),
        Int32.to_int (Binarray.get_int32 data (i * 8 + 8)))
  in
  (* Initialise internal representation *)
  let rgba = new_rgba width height in
  let bytes = linear_rgb rgba in
  Array1.fill bytes 0;
  (* Construct bitmap *)
  List.iter2
    (fun r (offset, length) ->
      if length > 0 then
        let block = Binarray.sub data offset length in
        if Binarray.get_i16 block 0x00 != 1 then failwith "undefined block type";
        let parts = Binarray.get_i16 block 0x02 in
      (*let block_x = Binarray.get_int block 0x04 ## not used currently
        and block_y = Binarray.get_int block 0x08
        and block_w = Binarray.get_int block 0x0c
        and block_h = Binarray.get_int block 0x10
        and block_ox = Binarray.get_int block 0x14
        and block_oy = Binarray.get_int block 0x18
        in*)
        (* ignore 0x1c..0x73 for now *)
        let i_offs = ref 0x74 in
        for j = 0 to parts - 1 do
          let px = Binarray.get_i16 block (!i_offs) + r.x1
          and py = Binarray.get_i16 block (!i_offs + 2) + r.y1
          and tr = Binarray.get_i16 block (!i_offs + 4)
          and pw = Binarray.get_i16 block (!i_offs + 6) * 4
          and ph = Binarray.get_i16 block (!i_offs + 8) in
          if !App.include_parts then r.parts <- r.parts @ [{ px = px; py = py; pw = pw / 4; ph = ph; trans = tr }];
          (* copy part line by line *)
          i_offs := !i_offs + 0x5c;
          for ly = py to py + ph - 1 do
            let dst = Array1.sub bytes ((px + ly * width) * 4) pw
            and src = Array1.sub block !i_offs pw in
            Array1.blit src dst;
            i_offs := !i_offs + pw
          done
        done)
    regions index;
  (* Return internal representation *)
  { width = width;
    height = height;
    data = `RGBA rgba;
    metadata =
      (match regions with
        | [{ x1 = 0; y1 = 0; x2 = x2; y2 = y2; ox = 0; oy = 0; parts = []}] when x2 = width - 1 && y2 = height - 1 -> None
        | _ -> Some (xml_of_regions regions)) }


let list_index value =
  let rec loop n =
    function
      | [] -> raise Not_found
      | x::xs -> if x = value then n else loop (n + 1) xs
  in
  loop 0

let encode_format_2 img i_regions rgba =
  assert (Array3.dim3 rgba = 4);
  (* Transform region list to label duplicates. *)
  let n_regions =
    List.mapi
      (fun i elt ->
        let idx = try list_index elt i_regions with Not_found -> max_int in
        if idx < i
        then `Dup idx
        else `Elt elt)
      i_regions
  in
  (* For non-duplicate regions, isolate graphical elements. *)
  let blocks =
    List.map
      (function (`Dup _) as d -> d | `Elt elt ->
        let pixel_clear x y =
          if !App.masked_only
          then rgba.{y, x, 3} != 0
          else rgba.{y, x, 0} != 0 || rgba.{y, x, 1} != 0 || rgba.{y, x, 2} != 0 || rgba.{y, x, 3} != 0
        in
        let x1 = ref elt.x1 and y1 = ref elt.y1 and x2 = ref elt.x2 and y2 = ref elt.y2 in
        List.iter (fun r -> if !r < 0 then r := 0) [x1; y1; x2; y2];
        List.iter (fun r -> if !r >= img.width then r := img.width - 1) [x1; x2];
        List.iter (fun r -> if !r >= img.height then r := img.height - 1) [y1; y2];
        let scanline_clear y =
          let rv = ref true and x = ref !x1 in
          while !x <= !x2 && !rv do
            if pixel_clear !x y
            then rv := false 
            else incr x
          done; !rv
        in
        let break = ref false in
        while !y1 <= !y2 && not !break do if scanline_clear !y1 then incr y1 else break := true done;
        break := false;
        while !y2 >= !y1 && not !break do if scanline_clear !y2 then decr y2 else break := true done;
        if !y1 > !y2 then `Elt { elt with x2 = elt.x1; y2 = elt.y1 } else
        let column_clear x =
          let rv = ref true and y = ref !y1 in
          while !y <= !y2 && !rv do
            if pixel_clear x !y
            then rv := false 
            else incr y
          done; !rv
        in
        let break = ref false in
        while !x1 <= !x2 && not !break do if column_clear !x1 then incr x1 else break := true done;
        break := false;
        while !x2 >= !x1 && not !break do if column_clear !x2 then decr x2 else break := true done;
        if !x1 > !x2 then failwith "sanity check failed: block is empty horizontally but not vertically";
        `Elt { x1 = !x1; y1 = !y1; x2 = !x2; y2 = !y2; ox = elt.ox; oy = elt.oy; parts = []})
      n_regions
  in
  (* Create uncompressed data. *)
  let regions = Array.of_list i_regions in
  let block_count = Array.length regions in
  let header_len = 4 + block_count * 8 in
  let dat_header = Binarray.create header_len
  and curr_offset = ref header_len in
  Binarray.put_int dat_header 0 block_count;
  let bytes = linear_rgb rgba in
  let blocks =
    List.mapi
      (fun i -> function
        | `Dup j
           -> let offset = Binarray.get_int dat_header (4 + j * 8)
              and length = Binarray.get_int dat_header (8 + j * 8) in
              Binarray.put_int dat_header (4 + i * 8) offset;
              Binarray.put_int dat_header (8 + i * 8) ~-length;
              None
        | `Elt elt
           -> let parts =
                (* Ideally, we would attempt to encode blocks efficiently by not specifying blank areas.
                   For now, an inefficient encoding will be just fine.  (Actually it seems not to be
                   significantly worse anyway...) *)
                let x = elt.x1 - regions.(i).x1 and y = elt.y1 - regions.(i).y1
                and w = elt.x2 - elt.x1 + 1 and h = elt.y2 - elt.y1 + 1 in
                let part = Binarray.create (0x5c + w * h * 4) in
                Binarray.put_i16 part 0x00 x;
                Binarray.put_i16 part 0x02 y;
                part.{0x04} <- 1; (* TODO: work out when 0 is appropriate! *)
                part.{0x05} <- 0; (* TODO: figure out what the hell this is! *)
                Binarray.put_i16 part 0x06 w;
                Binarray.put_i16 part 0x08 h;
                Binarray.fill (Binarray.sub part 0x0a 82) 0;
                for y = 0 to h - 1 do
                  Binarray.blit
                    (Binarray.sub bytes ((y + elt.y1) * img.width * 4 + elt.x1 * 4) (w * 4))
                    (Binarray.sub part (0x5c + y * w * 4) (w * 4))
                done;
                [part]
              in
              let block_len, part_count =
                List.fold_left
                  (fun (a, b) part -> a + Binarray.dim part, b + 1)
                  (0x74, 0) parts
              in
              let block = Binarray.create block_len in
              Binarray.put_i16 block 0x00 1;
              Binarray.put_i16 block 0x02 part_count;
              Binarray.put_int block 0x04 (elt.x1 - regions.(i).x1);
              Binarray.put_int block 0x08 (elt.y1 - regions.(i).y1);
              Binarray.put_int block 0x0c (elt.x2 - elt.x1 + 1);
              Binarray.put_int block 0x10 (elt.y2 - elt.y1 + 1);
              Binarray.put_int block 0x14 elt.ox;
              Binarray.put_int block 0x18 elt.oy;
              Binarray.put_int block 0x1c (regions.(i).x2 - regions.(i).x1 + 1);
              Binarray.put_int block 0x20 (regions.(i).y2 - regions.(i).y1 + 1);

              Binarray.fill (Binarray.sub block 0x24 80) 0;
              let sc =
                List.fold_left
                  (fun offs part ->
                    Binarray.blit part (Binarray.sub block offs (Binarray.dim part));
                    offs + Binarray.dim part)
                  0x74 parts
              in assert (sc = block_len);
              Binarray.put_int dat_header (4 + i * 8) !curr_offset;
              Binarray.put_int dat_header (8 + i * 8) block_len;
              let offs, len = !curr_offset, block_len in
              curr_offset := !curr_offset + block_len;
              Some (offs, len, block))
      blocks
  in
  (* Build and compress data. *)
  let data = Binarray.create (header_len + !curr_offset) in
  Binarray.blit dat_header (Binarray.sub data 0 header_len);
  List.iter
    (Option.may
      (fun (offs, len, arr) ->
        Binarray.blit arr (Binarray.sub data offs len)))
    blocks;
  let compressed_size = compress_1 data false (* for some reason !App.brutal produces LARGER images here *) in
  (* Build external representation. *)
  let arr = Binarray.create (9 + Array.length regions * 24 + 8 + compressed_size) in
  arr.{0} <- 2;
  Binarray.put_i16 arr 0x01 img.width;
  Binarray.put_i16 arr 0x03 img.height;
  Binarray.put_int arr 0x05 (Array.length regions);
  Array.iteri
    (fun i r ->
      Binarray.put_int arr (0x09 + i * 24 + 0x00) r.x1;
      Binarray.put_int arr (0x09 + i * 24 + 0x04) r.y1;
      Binarray.put_int arr (0x09 + i * 24 + 0x08) r.x2;
      Binarray.put_int arr (0x09 + i * 24 + 0x0c) r.y2;
      Binarray.put_int arr (0x09 + i * 24 + 0x10) r.ox;
      Binarray.put_int arr (0x09 + i * 24 + 0x14) r.oy)
    regions;
  Binarray.put_int arr (0x09 + Array.length regions * 24) (compressed_size + 8);
  Binarray.put_int arr (0x0d + Array.length regions * 24) (Binarray.dim data);
  Binarray.blit
    (Binarray.sub data 0 compressed_size)
    (Binarray.sub arr (0x11 + Array.length regions * 24) compressed_size);
  arr, None


(* General functions *)

let of_file fname =
  let arr = Binarray.read_input fname in
  let width = Binarray.get_i16 arr 0x01
  and height = Binarray.get_i16 arr 0x03 in
  let decoder =
    match arr.{0} with
      | 0 -> decode_format_0 (* 24-bit RGB *)
      | 1 -> decode_format_1 (* 8-bit paletted *)
      | 2 -> decode_format_2 (* funky 32-bit RGBA *)
      | _ -> raise (Unsupported ("unknown g00 format " ^ string_of_int arr.{0}))
  in
  decoder arr width height


let to_file img outname format =
  (* Parse metadata, if present. *)
  let default_format =
    match format with
      | "0" | "1" | "2" -> format
      | "" -> "auto"
      | _ -> Printf.ksprintf Optpp.sysWarning "unknown G00 format `%s'" format; "auto"
  and default_regions =
    [{ x1 = 0;
       y1 = 0;
       x2 = img.width - 1;
       y2 = img.height - 1;
       ox = 0;
       oy = 0;
       parts = []}]
  in
  let parse_metadata attrs elts =
    let parts_of_xml attrs =
      try { px = int_of_string (List.assoc "X" attrs);
            py = int_of_string (List.assoc "Y" attrs);
            pw = int_of_string (List.assoc "W" attrs);
            ph = int_of_string (List.assoc "H" attrs);
            trans = int_of_string (List.assoc "TRANS" attrs); }
      with Failure "int_of_string" -> raise (Bad_metadata "<part> coordinates must be integers")
    in
    let region_of_xml =
      function
        | Xml.PCData _ -> assert false
        | Xml.Element (id, attrs, children)
         -> assert (id = "region");
            let parts, ox, oy =
              List.fold_left
                (fun (p, ox, oy) -> function
                  | Xml.Element ("part", attrs, _) -> parts_of_xml attrs :: p, ox, oy
                  | Xml.Element ("origin", attrs, _)
                      -> (try p, int_of_string (List.assoc "X" attrs), int_of_string (List.assoc "Y" attrs)
                          with Failure "int_of_string" -> raise (Bad_metadata "<origin> coordinates must be integers"))
                  | _ -> p, ox, oy)
                ([], 0, 0)
                children
            in
            try
              { x1 = int_of_string (List.assoc "X1" attrs);
                y1 = int_of_string (List.assoc "Y1" attrs);
                x2 = int_of_string (List.assoc "X2" attrs);
                y2 = int_of_string (List.assoc "Y2" attrs);
                ox = ox;
                oy = oy;
                parts = parts; }
            with
              | Not_found -> raise (Bad_metadata "missing attribute in <region> tag")
              | Failure "int_of_string" -> raise (Bad_metadata "<region> coordinates must be integers")
    in
    let format =
      if default_format = "auto" then
      try
        List.assoc "FORMAT" attrs
      with Not_found ->
        assert false
      else default_format
    and regions =
      try
        let regions_xml = List.find (function Xml.Element ("regions", _, _) -> true | _ -> false) elts in
        Xml.map region_of_xml regions_xml
      with Not_found -> 
        default_regions
    in
    format, regions
  in
  let spec_fmt, regions =
    match img.metadata with
      | None -> default_format, default_regions
      | Some ("vas_g00.dtd", Xml.Element ("vas_g00", attrs, elts)) -> parse_metadata attrs elts
      | _ -> raise (Bad_metadata "g00 metadata must use the vas_g00 document type")
  in
  assert (regions <> []);
  (* Determine format to use for output, and hand over to format-specific encoder.
     If forced or specified in metadata, use that format regardless. *)
  let arr, xml =
    if spec_fmt = "0" then encode_format_0 img else
    let rgba = get_as_rgba img in
    match spec_fmt with
      | "0" -> assert false
      | "1" -> encode_format_1 img (get_unique_colours_rgba rgba) rgba
      | "2" -> encode_format_2 img regions rgba
      | _ -> (* If not specified, auto-detect the most appropriate format. *)
             assert (spec_fmt = "auto");
             (* If image has more than one region, use format 2. *)
             if List.tl regions <> [] then
               encode_format_2 img regions rgba
             else
               (* If image has <= 256 unique colours, use 1 *)
               try
                 encode_format_1 img (get_unique_colours_rgba rgba) rgba
               with e when e = too_many_colours ->
                 (* If image has no mask, use 0, else use 2. *)
                 match get_separate img with
                   | rgb, None -> encode_format_0 img
                   | _ -> encode_format_2 img regions rgba
  in
  Binarray.write_file arr outname;
  xml

let check_header ic =
(* G00s are particularly nasty due to the lack of any magic number:
   the only way is to check for consistency of the file size marker
   and hope nobody else picked the same file layout. *)
  try
    let s = String.create 8 in
    ignore (input ic s 0 8);
    let i = int_of_char s.[5]
        lor (int_of_char s.[6] lsl 8)
        lor (int_of_char s.[7] lsl 16)
        lor (input_byte ic lsl 24)
    in
    (match int_of_char s.[0] with
      | 0 | 1 -> if i <> in_channel_length ic - 5 then raise Exit
      | 2 -> seek_in ic (i * 24 + 9);
             really_input ic s 0 4;
             let j = int_of_char s.[0]
                 lor (int_of_char s.[1] lsl 8)
                 lor (int_of_char s.[2] lsl 16)
                 lor (int_of_char s.[2] lsl 24)
             in
             if j <> in_channel_length ic - (i * 24 + 9) then raise Exit
      | _ -> raise Exit);  
    true
  with Exit ->
    false

let _ =
  formats#register
    { name = "G00";
      extensions = ["G00"];
      of_file = of_file;
      to_file = to_file;
      check_header = check_header;
      use_meta = true;
      default_conv_ext = "PNG"; }
