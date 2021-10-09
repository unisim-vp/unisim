external decode : thumb:bool -> bigendian:bool -> itstate:int ->
  addr:int -> int -> string = "arm32dba_decode"

let decode ~thumb ?(bigendian=false) ?(itstate=0) ~addr code =
  decode ~thumb ~bigendian ~itstate ~addr code
