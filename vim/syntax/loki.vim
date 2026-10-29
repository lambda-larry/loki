if exists("b:current_syntax")
  finish
endif

let b:current_syntax = 'loki'

syntax keyword LokiNote  contained NOTE INFO PERF
syntax keyword LokiTodo  contained TODO WIP
syntax keyword LokiWarn  contained WARNING HACK FIX
syntax keyword LokiError contained ERROR BUG FIXME XXX

syntax keyword LokiConditional if
syntax keyword LokiConditional else
syntax keyword LokiConditional match
syntax keyword LokiConditional switch
syntax keyword LokiConditional case
syntax keyword LokiKeyword let
syntax keyword LokiKeyword mut
syntax keyword LokiKeyword data
syntax keyword LokiKeyword enum
syntax keyword LokiKeyword struct
syntax keyword LokiKeyword union
syntax keyword LokiKeyword type
syntax keyword LokiKeyword transmute
syntax keyword LokiKeyword import
syntax keyword LokiKeyword module
syntax keyword LokiKeyword package

syntax keyword LokiKeyword static
syntax keyword LokiKeyword extern
syntax keyword LokiKeyword intern

syntax keyword LokiKeyword syscall


syntax keyword LokiStatement goto
syntax keyword LokiStatement return
syntax keyword LokiStatement continue
syntax keyword LokiStatement break
syntax keyword LokiStatement default
syntax keyword LokiStatement recurse

syntax keyword LokiRepeat do
syntax keyword LokiRepeat loop

syntax keyword LokiType bool
syntax keyword LokiType char
syntax keyword LokiType void
syntax keyword LokiType u64 u32 u16 u8
syntax keyword LokiType i64 i32 i16 i8
syntax keyword LokiType b64 b32 b16 b8
syntax keyword LokiType u64be u32be u16be u8be
syntax keyword LokiType i64be i32be i16be i8be
syntax keyword LokiType u64le u32le u16le u8le
syntax keyword LokiType i64le i32le i16le i8le
syntax keyword LokiType u64ne u32ne u16ne u8ne
syntax keyword LokiType i64ne i32ne i16ne i8ne
syntax keyword LokiType f64 f32 f16

syntax keyword LokiType string matrix

syntax keyword LokiType ssize_t size_t ssize usize

syntax match LokiNumber "\<\d\+\('\d\+\)*\>"
syntax match LokiNumber "\<0x\x\+\('\x\+\)*\>"
syntax match LokiNumber "\<0b[0-1]\+\('[0-1]\+\)*\>"
syntax match LokiNumber "\<0o[0-7]\+\('[0-7]\+\)*\>"

syntax match LokiLabel       "[_a-zA-Z][0-9_a-zA-Z]* *:\ze[ \n\r]\+"
syntax match LokiConstant    "\<[_A-Z][0-9_A-Z]*\>"

syntax match LokiOperator "\~"
syntax match LokiOperator "!"
syntax match LokiOperator "&"
syntax match LokiOperator "|"
syntax match LokiOperator "+"
syntax match LokiOperator "-"
syntax match LokiOperator "*"
syntax match LokiOperator "/"
syntax match LokiOperator "%"
syntax match LokiOperator "="
syntax match LokiOperator "<"
syntax match LokiOperator ">"
syntax match LokiOperator "<="
syntax match LokiOperator ">="
syntax match LokiOperator "=="
syntax match LokiOperator "!="
syntax match LokiOperator "\~="
syntax match LokiOperator "\["
syntax match LokiOperator "\]"

syntax match LokiOperator ":="
" syntax match LokiOperator ":"

syntax keyword LokiOperator cast
" syntax keyword LokiOperator alignof
" syntax keyword LokiOperator containerof
" syntax keyword LokiOperator offsetof
" syntax keyword LokiOperator sizeof
" syntax keyword LokiOperator lenof

" syntax keyword LokiOperator range

" syntax keyword LokiOperator in
" syntax keyword LokiOperator as
" syntax keyword LokiOperator over
" syntax keyword LokiOperator of

syntax match LokiOperator "\.\.<"
syntax match LokiOperator "\.\.="

syntax match LokiSpecial "\.\.\."

" syntax keyword LokiBoolean true
" syntax keyword LokiBoolean false

syntax keyword LokiConstant nullptr

syntax match LokiPPDirective "@[a-z_]\+"

syntax region LokiComment start="//" end="$" contains=LokiTodo,LokiWarn,LokiNote,LokiError
syntax region LokiComment start="/\*" end="\*/"

syntax region LokiComment start="//! " end="$" contains=LokiPPDirective

syntax match LokiSpecial display contained "\\\%(x\x\+\|\o\{1,3}\|.\|$\)"

syntax region LokiString start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=LokiSpecial

syntax region LokiChar start=+'+ skip=+\\\\\|\\"+ end=+'+ contains=LokiSpecial

highlight clear LokiKeyword
highlight clear LokiStatement
highlight clear LokiRepeat
highlight clear LokiConditional
highlight clear LokiType
highlight clear LokiNumber
highlight clear LokiLabel
highlight clear LokiOperator
highlight clear LokiConstant
highlight clear LokiComment
highlight clear LokiBoolean
highlight clear LokiString
highlight clear LokiSpecial
highlight clear LokiChar
highlight clear LokiFunction
highlight clear LokiPPDirective
highlight clear LokiTodo
highlight clear LokiNote
highlight clear LokiWarn
highlight clear LokiError

highlight default link LokiKeyword     @keyword
highlight default link LokiStatement   @keyword
highlight default link LokiRepeat      @keyword.repeat
highlight default link LokiConditional @keyword.conditional
highlight default link LokiType        @type.builtin
highlight default link LokiNumber      @number
highlight default link LokiLabel       @label
highlight default link LokiOperator    @operator
highlight default link LokiConstant    @constant
highlight default link LokiComment     @comment
highlight default link LokiBoolean     @boolean
highlight default link LokiString      @string
highlight default link LokiSpecial     @string.escape
highlight default link LokiChar        @character
highlight default link LokiFunction    @function
highlight default link LokiPPDirective @keyword
highlight default link LokiTodo        @comment.todo
highlight default link LokiError       @comment.error
highlight default link LokiWarn        @comment.warning
highlight default link LokiNote        @comment.note

syntax keyword LokiFunction putchar
syntax keyword LokiFunction puts
syntax keyword LokiFunction printf
syntax keyword LokiFunction fprintf
syntax keyword LokiFunction fputc
syntax keyword LokiFunction malloc
syntax keyword LokiFunction calloc
syntax keyword LokiFunction free
syntax keyword LokiFunction fopen
syntax keyword LokiFunction fclose
syntax keyword LokiFunction sscanf
syntax keyword LokiFunction fgets
syntax keyword LokiFunction strcmp
syntax keyword LokiFunction strlen

syntax keyword LokiFunction memcmp
syntax keyword LokiFunction memcpy

syntax keyword LokiFunction log_init
syntax keyword LokiFunction log_fini

syntax keyword LokiFunction log_todo
syntax keyword LokiFunction log_info
syntax keyword LokiFunction log_debug
syntax keyword LokiFunction log_trace
syntax keyword LokiFunction log_warn
syntax keyword LokiFunction log_error
syntax keyword LokiFunction log_fatal
