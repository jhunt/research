;;
;; Scheme implementation of find-r
;; try:
;;
;;   gsi> (load "find-r.scm")
;;   > (find-r 3 256)
;;   36395321889/10000000000
;;   > (find-v (find-r 3 256) 256)
;;   .004838056816020526
;;
(define v+ 1.0e-5)
(define (integr fn r ep)
  (let ((y (fn r)))
    (if (< y ep)
      0
      (+ (* y v+) (integr fn (+ r v+) ep)))))

(define (f x)
  (exp (/ (* -1 x x) 2)))
(define (f- y)
  (sqrt (* -2 (log y))))

(define (find-v r ep)
  (+ (* r (f r))
     (integr f r ep)))

(define (find-xn x n m v)
  (if (eq? n m)
    x
    (find-xn (f- (+ (/ v x) (f x)))
             (- n 1)
             m v)))

(define (try-x r n)
  (let* ((v (find-v r 1.0e-3))
         (x (find-xn r (- n 1) 0 (find-v r 1.0e-3))))
    (+ (- v x)
       (* x (f x)))))

(define (larger r n d fitness)
  (let ((x (try-x r n))
        (d+ (/ d 10)))
    (cond
      ((< d 1e-10) r)
      ((fitness x) r)
      ((and (real? x) (not (nan? x)))
       (smaller (- r d+) n d+ fitness))
      (#t
       (larger  (+ r d) n d fitness)))))

(define (smaller r n d fitness)
  (let ((x (try-x r n))
        (d+ (/ d 10)))
    (cond
      ((< d 1e-10) r)
      ((fitness x) r)
      ((and (real? x) (not (nan? x)))
       (smaller (- r d) n d fitness))
      (#t
       (larger  (+ r d+) n d+ fitness)))))

(define (find-r r n)
  (smaller r n 1 (lambda (x)
                   (and (real? x)
                        (> x 0)
                        (< x 1.0e-10)))))
