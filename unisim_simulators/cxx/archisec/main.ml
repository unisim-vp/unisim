let () =
  match Sys.argv.(1) with
  | "x86" | "amd64" ->
    let m64 = Sys.argv.(1) = "amd64"
    and addr = int_of_string Sys.argv.(2)
    and opcode = Sys.argv.(3) in
    print_string @@ Amd64dba.decode ~m64 ~addr opcode
  | "arm" | "thumb" ->
    let thumb = Sys.argv.(1) = "thumb"
    and addr = int_of_string Sys.argv.(2)
    and opcode = int_of_string Sys.argv.(3) in
    print_string @@ Arm32dba.decode ~thumb ~addr opcode
  | "aarch64" ->
    let addr = int_of_string Sys.argv.(2)
    and opcode = int_of_string Sys.argv.(3) in
    print_string @@ Aarch64dba.decode ~addr opcode
  | _ -> failwith "Unknown arch"
