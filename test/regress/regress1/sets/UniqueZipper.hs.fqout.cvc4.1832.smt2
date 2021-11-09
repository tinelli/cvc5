(set-logic QF_ALL)
(set-info :status unsat)
(define-sort Elt () Int)
(define-sort mySet () (Set Elt ))
(define-fun smt_set_emp () mySet (as set.empty mySet))
(define-fun smt_set_mem ((x Elt) (s mySet)) Bool (set.member x s))
(define-fun smt_set_add ((s mySet) (x Elt)) mySet (set.union s (set.singleton x)))
(define-fun smt_set_cup ((s1 mySet) (s2 mySet)) mySet (set.union s1 s2))
(define-fun smt_set_cap ((s1 mySet) (s2 mySet)) mySet (set.intersection s1 s2))
;(define-fun smt_set_com ((s mySet)) mySet ((_ map not) s))
(define-fun smt_set_dif ((s1 mySet) (s2 mySet)) mySet (set.minus s1 s2))
;(define-fun smt_set_sub ((s1 mySet) (s2 mySet)) Bool (= smt_set_emp (smt_set_dif s1 s2)))
(define-fun smt_set_sub ((s1 mySet) (s2 mySet)) Bool (set.subset s1 s2))
(declare-fun z3v66 () Int)
(declare-fun z3v67 () Int)
(assert (distinct z3v66 z3v67))
(declare-fun z3v68 () Int)
(declare-fun z3f69 (Int) Int)
(declare-fun z3f70 (Int) mySet)
(declare-fun z3v71 () Int)
(declare-fun z3f72 (Int) mySet)
(declare-fun z3v73 () Int)
(declare-fun z3v74 () Int)
(declare-fun z3v75 () Int)
(declare-fun z3f76 (Int) Bool)
(declare-fun z3f77 (Int Int) Int)
(declare-fun z3v78 () Int)
(declare-fun z3f79 (Int) Bool)
(declare-fun z3v80 () Int)
(declare-fun z3f81 (Int) Int)
(declare-fun z3v82 () Int)
(declare-fun z3v83 () Int)
(declare-fun z3v85 () Int)
(declare-fun z3v86 () Int)
(declare-fun z3v87 () Int)
(declare-fun z3f88 () Int)
(declare-fun z3v89 () Int)
(declare-fun z3v90 () Int)
(declare-fun z3v91 () Int)
(declare-fun z3v92 () Int)
(declare-fun z3v93 () Int)
(declare-fun z3f94 (Int) Int)
(declare-fun z3f95 (Int) Int)
(declare-fun z3f96 (Int Int Int) Int)
(declare-fun z3v97 () Int)
(declare-fun z3v98 () Int)
(declare-fun z3v99 () Int)
(declare-fun z3v100 () Int)
(declare-fun z3v101 () Int)
(declare-fun z3v102 () Int)
(declare-fun z3v103 () Int)
(declare-fun z3v104 () Int)
(declare-fun z3v105 () Int)
(declare-fun z3v106 () Int)
(declare-fun z3v107 () Int)
(declare-fun z3v108 () Int)
(declare-fun z3v109 () Int)
(declare-fun z3v110 () Int)
(declare-fun z3v113 () Int)
(declare-fun z3v114 () Int)
(declare-fun z3v115 () Int)
(declare-fun z3v116 () Int)
(declare-fun z3v117 () Int)
(declare-fun z3v118 () Int)
(declare-fun z3v120 () Int)
(declare-fun z3v121 () Int)
(declare-fun z3v122 () Int)
(declare-fun z3v124 () Int)
(declare-fun z3v125 () Int)
(declare-fun z3v126 () Int)
(declare-fun z3v127 () Int)
(declare-fun z3v128 () Int)
(declare-fun z3v129 () Int)
(declare-fun z3v131 () Int)
(declare-fun z3v132 () Int)
(declare-fun z3v133 () Int)
(declare-fun z3v134 () Int)
(declare-fun z3v135 () Int)
(declare-fun z3v136 () Int)
(declare-fun z3v137 () Int)
(declare-fun z3v138 () Int)
(declare-fun z3v139 () Int)
(declare-fun z3v140 () Int)
(declare-fun z3v141 () Int)
(declare-fun z3v142 () Int)
(declare-fun z3v144 () Int)
(declare-fun z3v145 () Int)
(declare-fun z3v146 () Int)
(declare-fun z3v149 () Int)
(declare-fun z3v151 () Int)
(declare-fun z3v154 () Int)
(declare-fun z3v155 () Int)
(declare-fun z3v156 () Int)
(declare-fun z3v157 () Int)
(declare-fun z3v158 () Int)
(declare-fun z3v159 () Int)
(declare-fun z3v161 () Int)
(declare-fun z3v163 () Int)
(declare-fun z3v164 () Int)
(declare-fun z3v165 () Int)
(declare-fun z3v167 () Int)
(declare-fun z3v170 () Int)
(declare-fun z3v174 () Int)
(declare-fun z3v175 () Int)
(declare-fun z3v176 () Int)
(declare-fun z3v179 () Int)
(declare-fun z3v181 () Int)
(declare-fun z3v182 () Int)
(declare-fun z3v183 () Int)
(declare-fun z3v184 () Int)
(declare-fun z3v187 () Int)
(declare-fun z3v188 () Int)
(declare-fun z3v189 () Int)
(declare-fun z3v190 () Int)
(declare-fun z3f191 (Int) Int)
(declare-fun z3f192 (Int) Int)
(declare-fun z3v195 () Int)
(declare-fun z3v196 () Int)
(declare-fun z3v199 () Int)
(declare-fun z3v200 () Int)
(declare-fun z3v201 () Int)
(declare-fun z3v202 () Int)
(declare-fun z3v203 () Int)
(declare-fun z3v206 () Int)
(declare-fun z3v207 () Int)
(declare-fun z3v208 () Int)
(declare-fun z3v210 () Int)
(declare-fun z3v211 () Int)
(declare-fun z3v212 () Int)
(declare-fun z3f213 (Int) Bool)
(declare-fun z3f214 (Int) Int)
(declare-fun z3v215 () Int)
(declare-fun z3v216 () Int)
(declare-fun z3v218 () Int)
(declare-fun z3v220 () Int)
(declare-fun z3v221 () Int)
(declare-fun z3v222 () Int)
(declare-fun z3v223 () Int)
(declare-fun z3v224 () Int)
(declare-fun z3v225 () Int)
(declare-fun z3v226 () Int)
(declare-fun z3v227 () Int)
(declare-fun z3v228 () Int)
(declare-fun z3v229 () Int)
(declare-fun z3v230 () Int)
(declare-fun z3v231 () Int)
(declare-fun z3v233 () Int)
(declare-fun z3v236 () Int)
(declare-fun z3v242 () Int)
(declare-fun z3v243 () Int)
(declare-fun z3v244 () Int)
(declare-fun z3v245 () Int)
(declare-fun z3v247 () Int)
(declare-fun z3v248 () Int)
(declare-fun z3v249 () Int)
(declare-fun z3v250 () Int)
(declare-fun z3v251 () Int)
(declare-fun z3v252 () Int)
(declare-fun z3v253 () Int)
(declare-fun z3v255 () Int)
(declare-fun z3v256 () Int)
(declare-fun z3v258 () Int)
(declare-fun z3v259 () Int)
(declare-fun z3v261 () Int)
(declare-fun z3v262 () Int)
(declare-fun z3v263 () Int)
(declare-fun z3v264 () Int)
(declare-fun z3v266 () Int)
(declare-fun z3v268 () Int)
(declare-fun z3v270 () Int)
(declare-fun z3v271 () Int)
(declare-fun z3v272 () Int)
(declare-fun z3v274 () Int)
(declare-fun z3v275 () Int)
(declare-fun z3v276 () Int)
(declare-fun z3v278 () Int)
(declare-fun z3v279 () Int)
(declare-fun z3v281 () Int)
(declare-fun z3v282 () Int)
(declare-fun z3v283 () Int)
(declare-fun z3v284 () Int)
(declare-fun z3v285 () Int)
(declare-fun z3v287 () Int)
(declare-fun z3v289 () Int)
(declare-fun z3v290 () Int)
(declare-fun z3v291 () Int)
(declare-fun z3v292 () Int)
(declare-fun z3v293 () Int)
(declare-fun z3v296 () Int)
(declare-fun z3v298 () Int)
(declare-fun z3v299 () Int)
(declare-fun z3f300 (Int Int) Int)
(declare-fun z3v301 () Int)
(declare-fun z3v302 () Int)
(declare-fun z3v303 () Int)
(declare-fun z3v304 () Int)
(declare-fun z3v308 () Int)
(declare-fun z3v309 () Int)
(declare-fun z3v310 () Int)
(declare-fun z3v314 () Int)
(declare-fun z3v315 () Int)
(declare-fun z3v316 () Int)
(declare-fun z3v317 () Int)
(declare-fun z3v318 () Int)
(declare-fun z3v319 () Int)
(declare-fun z3v320 () Int)
(declare-fun z3v324 () Int)
(declare-fun z3v325 () Int)
(declare-fun z3v326 () Int)
(declare-fun z3v327 () Int)
(declare-fun z3v328 () Int)
(declare-fun z3v330 () Int)
(declare-fun z3v332 () Int)
(declare-fun z3v333 () Int)
(declare-fun z3v334 () Int)
(declare-fun z3v335 () Int)
(declare-fun z3v336 () Int)
(declare-fun z3v337 () Int)
(declare-fun z3v339 () Int)
(declare-fun z3v340 () Int)
(declare-fun z3v341 () Int)
(declare-fun z3v342 () Int)
(assert (= z3v342 z3v113))
(assert (>= (z3f69 z3v342) 0))
(assert (and (>= (z3f69 z3v113) 0) (>= (z3f69 z3v114) 0) (= (z3f72 z3v114) smt_set_emp) (= (z3f70 z3v114) smt_set_emp) (= (z3f69 z3v114) 0) (= (z3f76 z3v114) true) (>= (z3f69 z3v115) 0) (= (z3f72 z3v115) (ite (smt_set_mem z3v116 (z3f70 z3v113)) (smt_set_cup (smt_set_add smt_set_emp z3v116) (z3f72 z3v113)) (z3f72 z3v113))) (= (z3f70 z3v115) (smt_set_cup (smt_set_add smt_set_emp z3v116) (z3f70 z3v113))) (= (z3f69 z3v115) (+ 1 (z3f69 z3v113))) (= (z3f76 z3v115) false) (= z3v115 (z3f77 z3v116 z3v113)) (>= (z3f69 z3v115) 0) (= z3v115 z3v117) (>= (z3f69 z3v115) 0) (= (z3f72 z3v115) smt_set_emp) (>= (z3f69 z3v117) 0) (= (z3f72 z3v117) smt_set_emp) (z3f79 z3v66) (= (z3f81 z3v80) z3v80) (= (z3f81 z3v82) z3v82) (not (z3f79 z3v67)) (= (z3f81 z3v83) z3v83)))
(assert (not (and (= (z3f72 z3v342) smt_set_emp) (not (smt_set_mem z3v116 (z3f70 z3v342))))))
(check-sat)
