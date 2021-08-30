let () =
  let mode = Sys.argv.(1)
  and addr = Sys.argv.(2)
  and opcode = Sys.argv.(3) in
  Unisim_archisec.Decoder.decode ~mode ~addr opcode
