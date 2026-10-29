if exists("b:did_ftplugin")
  finish
endif

let b:did_ftplugin = 1
let b:undo_ftplugin = "setlocal comments< commentstring< formatoptions<"

setlocal comments=:///,://
setlocal commentstring=//\ %s

" setlocal format
setlocal formatoptions-=t " No Auto-wrap text using 'textwidth'

setlocal formatoptions+=c " Auto-wrap comment using 'textwidth'
setlocal formatoptions+=r " Automatically insert current comment leader after hitting <CR> in insert mode
setlocal formatoptions+=o " Automatically insert the current comment leader after hitting 'o' or 'O' in Normal mode
setlocal formatoptions+=q " Allow formatting of comments with "gq"
setlocal formatoptions+=l " Long lines are not broken in insert mode

setlocal include=^\s*//!\s*@lc_include
