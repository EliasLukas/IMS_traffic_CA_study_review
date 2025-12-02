From paper:


experiment symmetric vs asymetric
fig 1:
    symmetric
    road len 12000
    density 0.09
    l = lo = car velocity + 1
    lo back = v max
    p change = 1

    shows 2 plots, 1 for each lane (left, right)
    shows 400 sites in 400 time steps

fig 2:
    assymetric fig 1 setup
    road len 12000
    density 0.09
    l = lo = car velocity + 1
    lo back = v max
    p change = 1

    shows 2 plots, 1 for each lane (left, right)
    shows 400 sites in 400 time steps


simulation setup for further experiments

Lane lenght  = 133,33 sites
closed boundary (loop)
data extraction started after 1000 timesteps
statistics gathering every fifth step
space-time averaged flow:
    flow = average velocity over space and time
    implementation for specific lane - every fifth time-step
                    sum up all velocities present in a specific lane
                    at the end, multiply sums by 1/T * 1/(L)

    pingpong lane changes - same calculation but gathered every timestep,

fig 3 (pchange = 1, lookback = 5)
    flow rate with respect to global density
    density from 0.1 to 0.5 in step 0.01
    both - asymetric, (LF + RF) / 2
    left - asym LF
    right - asym RF
    symmetric - symme bothflows / 2

fig 4
    plotting asymmetric, symmetric and single flow to density ratio

fig 5
    y = lane changes per site and time step
    calculate as: total lane changes / (lane count * lane length * time steps)
    x = density
    plotted 4 lines:
        asym change prob = 0.5
                         = 1.0
        sym change prob = 0.5
                        = 1.0

fig 6:
    (fig 5 y / density)

fig 7 - 9 pingpong
fig 7 flow to density(0.07-0.15 step = 0.1), plot asym sym with pchange {0.5, 1}


look distance changes
flow to densities
fig 10 change from l = v+1 to l = v

fig 11 pchange = 1, lookback for sym and asym for LB = 0 / 4 

fig 12,13 = 400x400 lookback = 0 road in time evolution (like 1,2) for sym and asym left right lanes



            