# deck
provide-module -override deck %{
	add-highlighter shared/deck regions
	add-highlighter shared/deck/other default-region group

	# comment
	add-highlighter shared/deck/comment region '#!' '$' group
	add-highlighter shared/deck/comment/ fill comment

	# string
	add-highlighter shared/deck/string region '"' (?<!\\)(\\\\)*" group
	add-highlighter shared/deck/string/ fill string

	# values/operators
	evaluate-commands %sh{
		values='\d+'
		operators='#|\?|\|>|>\||\+|-|\*|/|%|<<|>>|&|\||\^|~|=|!=|<|<=|>|>=|\{|\}|\[|\]'
		addr='&\s*[^\s]*'
		label='@\s*[^\s]*'
		call='[^\s]*'
		builtins='let|go|here|word|byte|pop|get|set|mget|mset|::'

		printf %s "
			add-highlighter shared/deck/other/ regex (\s|^)\K("${values}")(\s|$)(("${values}")(\s|$))* 0:value
			add-highlighter shared/deck/other/ regex (\s|^)\K("${operators}")(\s|$)(("${operators}")(\s|$))* 0:operator
			add-highlighter shared/deck/other/ regex (\s|^)\K("${addr}")(\s|$)(("${addr}")(\s|$))* 0:value
			add-highlighter shared/deck/other/ regex (\s|^)\K("${label}")(\s|$)(("${label}")(\s|$))* 0:meta
			add-highlighter shared/deck/other/ regex (\s|^)\K("${call}")(\s|$)(("${call}")(\s|$))* 0:identifier
			add-highlighter shared/deck/other/ regex (\s|^)\K("${builtins}")(\s|$)(("${builtins}")(\s|$))* 0:builtin
		"
	}
}

hook global BufCreate .*\.(dk|deck) %{ set-option buffer filetype deck }
hook global WinSetOption filetype=deck %{ require-module deck }

hook -group deck-highlight global WinSetOption filetype=deck %{
	add-highlighter window/deck ref deck
	hook -once -always window WinSetOption filetype=.* %{ remove-highlighter window/deck }
}

hook global BufSetOption filetype=deck %{
	set-option buffer comment_line '#!'
	set-option buffer comment_block_begin '#!'
	set-option buffer comment_block_end ''
}

