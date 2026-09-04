if exists("b:current_syntax")
  finish
endif

syn keyword ninKeyword let fun return if else while for class new this async coroutine await global const mutex free override super break continue
syn keyword ninBoolean true false
syn keyword ninNil nil
syn keyword ninBuiltin print input system clock str num type len push pop import argv

syn match ninNumber '\<\d\+\(\.\d*\)\?\>'
syn match ninComment '//.*$'
syn region ninString start='"' end='"' skip='\\"'
syn region ninBlockComment start='/\*' end='\*/'

hi def link ninKeyword Keyword
hi def link ninBoolean Boolean
hi def link ninNil Constant
hi def link ninBuiltin Function
hi def link ninNumber Number
hi def link ninComment Comment
hi def link ninString String
hi def link ninBlockComment Comment

let b:current_syntax = "nin"
