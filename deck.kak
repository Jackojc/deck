# deck
provide-module -override deck %{
	add-highlighter shared/deck regions
	add-highlighter shared/deck/other default-region group

	# comment
	add-highlighter shared/deck/comment region '#!' '$' group
	add-highlighter shared/deck/comment/ fill comment

	# values/operators
	evaluate-commands %sh{
		values='true|false|\d+'
		operators='\+|-|\*|/|<<|>>|\+\+|--|&|\||\^|~|=|!=|<|<=|>|>=|#|\?|\.|&\.|>\||\|>|\[|\]'
		addr='&[\w\.][\w\d_]*'
		label='@[\w\.][\w\d_]*'
		call='[\w\.][\w\d_]*'
		builtins='get|set|mget|mset|and|or|xor|not|neg|abs|min|max|clear|here|if|br|pop|swap|dup|over|nip|tuck|rotl|rotr|call'

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

