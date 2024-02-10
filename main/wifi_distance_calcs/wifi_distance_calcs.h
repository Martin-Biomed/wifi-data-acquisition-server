

// This function calculates the Free Space Path Loss (FSPL)
// Refer to: https://www.electronics-notes.com/articles/antennas-propagation/propagation-overview/free-space-path-loss.php

// The full equation is: FSPL = 20log(d) + 20log(f) + 32.44 - Gtx - Grx [d = distance in km, f = MHz]

// A Wi-Fi Antenna (specifically that of the ESP32) is designed to be omnidirectional (no directional gain).
// An assumption is made that both wireless APs are using omnidirectional antennas (Gtx = Grx = 0)

// The following equation is derived:
// Received Power = Transmitted Power - FSPL - Attenuation Losses

// The max transmit power for 2.4GHz tends to be: 12dBm (phones) - 20dBm (dedicated APs)
// Refer to: https://metis.fi/en/2017/10/txpower/

float fspl(float Ptx, )