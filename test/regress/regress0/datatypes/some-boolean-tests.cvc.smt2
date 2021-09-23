; EXPECT: sat
(set-logic ALL)
(set-option :incremental false)
(declare-datatypes ((list 1)) ((par (T)((cons (car T) (care Bool) (cdr (list T))) (nil)))))
(declare-fun x () (list Real))
(declare-fun y () (Tuple Int Bool Int))
(assert (= y (mkTuple 5 true 4)))
(assert ((_ tupSel 1) y))
(assert (= x ((as cons (list Real)) (/ 11 10) true (as nil (list Real)))))
(check-sat)
