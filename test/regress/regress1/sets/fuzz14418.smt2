; symptom: assertion failure in EqEngine : hasTerm(t)
; 
; issue: had some nodes in d_pending, even though sat context had been popped,
; and those were no longer relevant.
;
; fix: make pending queues sat context depending. d_pendingEverInserted which
; is still user-context dependent takes care of not generating a lemma twice.
;
; sat
(set-info :source |fuzzsmt|)
(set-info :smt-lib-version 2.6)
(set-info :category "random")
(set-info :status sat)
(set-logic QF_UFLIAFS)
(define-sort Element () Int)
(declare-fun f0 ( Int) Int)
(declare-fun f1 ( (Set Element) (Set Element) (Set Element)) (Set Element))
(declare-fun p0 ( Int Int) Bool)
(declare-fun p1 ( (Set Element)) Bool)
(declare-fun v0 () Int)
(declare-fun v1 () (Set Element))
(declare-fun v2 () (Set Element))
(declare-fun v3 () (Set Element))
(declare-fun v4 () (Set Element))
(assert (let ((e5 7))
(let ((e6 (* e5 v0)))
(let ((e7 (* v0 e5)))
(let ((e8 (f0 e6)))
(let ((e9 (* v0 (- e5))))
(let ((e10 (f0 v0)))
(let ((e11 (* (- e5) e10)))
(let ((e12 (ite (p0 e7 e6) 1 0)))
(let ((e13 (set.union v3 v4)))
(let ((e14 (set.minus v2 v2)))
(let ((e15 (f1 v1 v4 v1)))
(let ((e16 (f1 e14 v1 v4)))
(let ((e17 (set.intersection e16 e15)))
(let ((e18 (f1 v4 e15 v2)))
(let ((e19 (ite (p1 e13) (set.singleton 1) (set.singleton 0))))
(let ((e20 (set.member v0 e17)))
(let ((e21 (set.member e7 e16)))
(let ((e22 (set.member e10 e16)))
(let ((e23 (set.member e8 e17)))
(let ((e24 (set.member e9 e14)))
(let ((e25 (set.member e8 e16)))
(let ((e26 (set.member v0 e13)))
(let ((e27 (set.member e12 v4)))
(let ((e28 (set.member e8 e14)))
(let ((e29 (set.member e8 v1)))
(let ((e30 (set.member e10 e13)))
(let ((e31 (set.member e7 e13)))
(let ((e32 (f1 e13 e13 e13)))
(let ((e33 (f1 e18 v4 e17)))
(let ((e34 (f1 v2 v2 e15)))
(let ((e35 (f1 e33 e18 e15)))
(let ((e36 (f1 e19 e14 e17)))
(let ((e37 (f1 e34 e18 e34)))
(let ((e38 (f1 v3 e34 e18)))
(let ((e39 (f1 e16 v4 e13)))
(let ((e40 (f1 v1 e34 e15)))
(let ((e41 (< e10 e11)))
(let ((e42 (= e6 e12)))
(let ((e43 (> e6 e11)))
(let ((e44 (< e12 e8)))
(let ((e45 (< e7 e10)))
(let ((e46 (= e11 e12)))
(let ((e47 (= e11 e7)))
(let ((e48 (<= e11 e10)))
(let ((e49 (p0 e9 e9)))
(let ((e50 (>= v0 e10)))
(let ((e51 (ite e22 e14 e33)))
(let ((e52 (ite e45 e16 e37)))
(let ((e53 (ite e42 e39 e17)))
(let ((e54 (ite e21 e39 e33)))
(let ((e55 (ite e29 e13 e13)))
(let ((e56 (ite e48 e15 e34)))
(let ((e57 (ite e50 e38 e53)))
(let ((e58 (ite e47 e32 v1)))
(let ((e59 (ite e20 e36 e33)))
(let ((e60 (ite e28 e35 v2)))
(let ((e61 (ite e48 e40 e38)))
(let ((e62 (ite e30 e38 e53)))
(let ((e63 (ite e22 v4 e19)))
(let ((e64 (ite e46 e60 e53)))
(let ((e65 (ite e25 e61 e16)))
(let ((e66 (ite e23 v3 e38)))
(let ((e67 (ite e49 v4 e18)))
(let ((e68 (ite e21 e54 v3)))
(let ((e69 (ite e25 e15 v4)))
(let ((e70 (ite e20 e55 e19)))
(let ((e71 (ite e27 e38 e36)))
(let ((e72 (ite e28 e14 e33)))
(let ((e73 (ite e42 e66 e60)))
(let ((e74 (ite e26 e54 e69)))
(let ((e75 (ite e28 e68 e71)))
(let ((e76 (ite e24 e33 e52)))
(let ((e77 (ite e44 e40 e74)))
(let ((e78 (ite e48 e32 e51)))
(let ((e79 (ite e22 e34 e62)))
(let ((e80 (ite e22 e78 e73)))
(let ((e81 (ite e22 e13 e55)))
(let ((e82 (ite e43 e37 e70)))
(let ((e83 (ite e48 e59 e80)))
(let ((e84 (ite e29 e15 e77)))
(let ((e85 (ite e41 e19 e35)))
(let ((e86 (ite e22 e63 e69)))
(let ((e87 (ite e26 e19 e70)))
(let ((e88 (ite e46 e37 e53)))
(let ((e89 (ite e25 e70 e76)))
(let ((e90 (ite e31 v4 e73)))
(let ((e91 (ite e46 e12 e8)))
(let ((e92 (ite e43 e11 e6)))
(let ((e93 (ite e50 e10 e7)))
(let ((e94 (ite e21 e8 e7)))
(let ((e95 (ite e27 v0 e6)))
(let ((e96 (ite e24 e9 e92)))
(let ((e97 (ite e22 e6 e92)))
(let ((e98 (ite e49 e96 e12)))
(let ((e99 (ite e27 e98 e6)))
(let ((e100 (ite e31 e11 e8)))
(let ((e101 (ite e26 e12 v0)))
(let ((e102 (ite e22 e92 e96)))
(let ((e103 (ite e28 e92 e6)))
(let ((e104 (ite e27 e12 v0)))
(let ((e105 (ite e23 e101 e9)))
(let ((e106 (ite e47 e11 e104)))
(let ((e107 (ite e45 e105 e100)))
(let ((e108 (ite e48 e12 e9)))
(let ((e109 (ite e42 e96 e91)))
(let ((e110 (ite e29 e11 e101)))
(let ((e111 (ite e50 e107 e110)))
(let ((e112 (ite e29 e104 e92)))
(let ((e113 (ite e20 e108 e12)))
(let ((e114 (ite e44 e96 e104)))
(let ((e115 (ite e41 e105 e110)))
(let ((e116 (ite e41 e6 e103)))
(let ((e117 (ite e28 e92 e114)))
(let ((e118 (ite e30 e111 e113)))
(let ((e119 (ite e30 e109 e8)))
(let ((e120 (ite e25 e12 e118)))
(let ((e121 (xor e46 e42)))
(let ((e122 (xor e28 e29)))
(let ((e123 (= e122 e49)))
(let ((e124 (and e43 e45)))
(let ((e125 (or e121 e23)))
(let ((e126 (and e125 e24)))
(let ((e127 (= e41 e126)))
(let ((e128 (xor e124 e44)))
(let ((e129 (not e26)))
(let ((e130 (= e22 e123)))
(let ((e131 (not e20)))
(let ((e132 (and e127 e27)))
(let ((e133 (=> e50 e131)))
(let ((e134 (=> e132 e30)))
(let ((e135 (xor e128 e48)))
(let ((e136 (ite e129 e47 e129)))
(let ((e137 (and e133 e130)))
(let ((e138 (or e136 e134)))
(let ((e139 (and e31 e31)))
(let ((e140 (not e137)))
(let ((e141 (= e140 e139)))
(let ((e142 (= e25 e21)))
(let ((e143 (not e142)))
(let ((e144 (and e143 e135)))
(let ((e145 (and e144 e138)))
(let ((e146 (and e145 e145)))
(let ((e147 (= e141 e146)))
e147
))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))

(check-sat)
