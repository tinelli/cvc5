; COMMAND-LINE: --ext-rewrite-quant -q
; EXPECT: sat
(set-logic NIA)
(assert (exists ((x Int)) (= (div 1 x x) x)))
(check-sat)
