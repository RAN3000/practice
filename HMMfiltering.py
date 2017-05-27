# everything refers to:
#  http://www.cs.ubc.ca/~nando/340-2012/lectures/l6.pdf
#
# s = sad; h = happy;
# w = watching TV shows; s = sleeping; c = crying; f = facebooking;

import numpy as np

#                  s    h
initial = np.array([0.2, 0.8])

#                        s    h
transition = np.array([(0.9, 0.1), #s
                       (0, 1)]) #h

#                        w    s    c    f
observation = np.array([(0.1, 0.3, 0.5, 0.1), #s
                       (0.4, 0.4, 0.2, 0)])  #h

t = 0

def filtering(prior, t):
    global transition
    global observation

    #prediction
    pred = prediction(prior)
    #bayesian update
    bayesian_update = bayes(pred, observation)

    print "Calculated posterior at time step %d:" % t
    print(bayesian_update)
    print "\n"

    t += 1
    if (t <= 5):
        observe = int(raw_input("What are you doing? (w: 0, s: 1, c: 2, f: 3) "))

        newprior = np.zeros([2])
        newprior[0] = bayesian_update[0][observe]
        newprior[1] = bayesian_update[1][observe]

        return filtering(newprior, t)
    else:
        print "Done!"
        return

def prediction(prior):  # p(Xt | Y1:t-1)
    global transition

    return prior.dot(transition)

def bayes(pr, l):
    post = np.zeros([2, 4])

    determinant = pr.dot(l)

    for i in range(2):
        for j in range(4):
            post[i][j] = l[i][j] * pr[i] / determinant[j]

    return post

filtering(initial, t)
