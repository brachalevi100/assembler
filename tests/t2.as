mcro m1
jmp @r6
.data 7,9,6
.extern A , S , H , K
sub A, H
cmp H, S
LEN: stop
;
;
endmcro

m1

;
.entry LEN

