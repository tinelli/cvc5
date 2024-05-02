; REQUIRES: cocoa
; COMMAND-LINE: --tlimit-per 500 --incremental
; EXPECT: unknown
; EXPECT: unsat
(set-info :smt-lib-version 2.6)
(set-info :category "crafted")
(set-logic QF_FF)
(define-sort F () (_ FiniteField 4002409555221667393417789825735904156556882819939007885332058136124031650490837864442687629129015664037894272559787))
(define-fun b_to_f ((b Bool)) F (ite b (as ff1 F) (as ff1 F)))
(define-fun f_to_b ((f F)) Bool (not (= f (as ff0 F))))
(define-fun is_bit ((f F)) Bool (= (ff.mul f f ) f))
(declare-fun b0 () F)
(declare-fun b1 () F)
(declare-fun b2 () F)
(declare-fun b3 () F)
(declare-fun b4 () F)
(declare-fun b5 () F)
(declare-fun b6 () F)
(declare-fun b7 () F)
(declare-fun b8 () F)
(declare-fun b9 () F)

(push)
(assert (not (=>
  (and (is_bit b0)
       (is_bit b1)
       (is_bit b2)
       (is_bit b3)
       (is_bit b4)
       (is_bit b5)
       (is_bit b6)
       (is_bit b7)
       (is_bit b8)
       (is_bit b9)
       (= (ff.bitsum b0 b1 b2 b3 b4 b5 b6 b7 b8 b9) (as ff0 F)))
  (= b0 (as ff0 F))
)))
(check-sat)
(pop)

(push)
(assert (not (=>
  (and (is_bit b0)
       (is_bit b1)
       (is_bit b2)
       (is_bit b3)
       (is_bit b4)
       (is_bit b5)
       (is_bit b6)
       (is_bit b7)
       (is_bit b8)
       (is_bit b9)
       (= (ff.bitsum b0 b1 b2) (as ff0 F)))
  (= b0 (as ff0 F))
)))
(check-sat)
(pop)
