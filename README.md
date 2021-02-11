
# Wormhole Raytracer
Implementation of paper [Visualizing Interstellar's Wormhole](https://arxiv.org/pdf/1502.03809.pdf)\
For interactive version [https://github.com/zyddnys/wormhole-realtime](https://github.com/zyddnys/wormhole-realtime)

# Some tricks
- Only rays alone the equatorial plane is traced, special thanks to [Scott Manley](https://www.youtube.com/user/szyzyg/)

# How to use
- Change wormhole parameters (length, mass, radius) in [main.cc](main.cc):120
- Change image parameters (resolution, spp) in [main.cc](main.cc):18
- Change runge-kutta integration parameters (step size, total steps) in [WormholeODE.h](WormholeODE.h):214

# Images
![](result.png)
![](result.bmp)
![](result2.bmp)
