
import bilevel_solver


bilevel_solver.setup_solver([[0.5,0.25,4]],[[1,1,10]],[[1,3,20]],[2,3])
bilevel_solver.solve(3)
