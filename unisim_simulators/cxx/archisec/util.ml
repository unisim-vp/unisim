type dll_handle
type dll_mode = For_checking | For_execution

external dll_open : dll_mode -> string -> dll_handle = "caml_dynlink_open_lib"

let ld_library_path_contents () =
  match Sys.getenv "CAML_LD_LIBRARY_PATH" with
  | exception Not_found ->
      []
  | s -> String.split_on_char (match Sys.os_type with "Win32" -> ';' | _ -> ':') s

let find_in_path path name =
  if not (Filename.is_implicit name) then
    if Sys.file_exists name then name else raise Not_found
  else begin
    let rec try_dir = function
        [] -> raise Not_found
      | dir::rem ->
        let fullname = Filename.concat dir name in
        if Sys.file_exists fullname then fullname else try_dir rem
    in try_dir path
  end

let () =
  let name =  "dllutil" ^ ".so" (* Config.ext_dll *) in
  let fullname =
    try
      let fullname = find_in_path (ld_library_path_contents ()) name in
      if Filename.is_implicit fullname then
        Filename.concat Filename.current_dir_name fullname
      else fullname
    with Not_found -> name in
  ignore @@ dll_open For_execution fullname
