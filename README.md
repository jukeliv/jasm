mw - write a value in mem ( X )

mr - read the value fo mem ( X ) and write it to mem ( Y )

add - add the value of mem ( Y ) to mem ( X )

sub - subtract the value of mem ( Y ) to mem ( X )

call - calls the interpreter to execute value X, available:
        0: exit program with mem ( 500 ) as exit value ( error is 2, good is 1, neutral 0 )

jmp - jump to ( X ) place in the code

jmpz - jump to ( X ) place in the code if mem ( Y ) is equal to 0

cmp - compares mem ( X ) and mem ( Y ) and if they are equal, saves 0 in mem ( 501 ), else, saves 1 in mem ( 501 )