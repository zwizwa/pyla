/* uart.c - UART analyzer
 *
 * (C) 2013-2014 by Tom Schouten <tom@zwizwa.be>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 
 *  as published by the Free Software Foundation
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "uart.h"

uart::uart() {
  _br = 9600;
  _sr = 4000000;
  _channel = 0;
  _update_clock_div();
  reset();
}
void uart::reset() {
}
void uart::_update_clock_div() {
  double div = _sr / _br;
  _clock_div = (unsigned int)(div + 0.5);
}
void uart::set_samplerate(double sr) {
  _sr = sr;
  _update_clock_div();
}
void uart::set_baudrate(double br) {
  _br = br;
  _update_clock_div();
}
void uart::set_channel(int channel) {
  _channel = channel;
  _update_clock_div();
}

std::vector<unsigned char> uart::analyze(std::vector<unsigned char> input) {
  std::vector<unsigned char> output;
  int i, i_size = input.size();
  for (i=0; i<i_size; i++) {
    int bit = (input[i] >> _channel) & 1;

    switch (_state) {

    case sm_idle:
      if (bit) break; // still idle

      // Skip start bit and sample first bit in the middle.
      _set_state(sm_sample);
      _bits_data = 0;
      _bits_count = 0;
      _bits_parity = 0;
      _delay = _clock_div + (_clock_div >> 1);
      break;

    case sm_break:
      if (!bit) break; // still break
      _set_state(sm_idle);
      break;

    case sm_sample:
      if (_delay > 0) {
        _delay--;
      }
      else {
        switch(_bits_count) {

        case 9: // STOP BIT
          if (!bit) {
            _log("F");
            // _set_state(sm_break);  // FIXME: break condition not implemented
          }
          output.push_back(_bits_data);
          _set_state(sm_idle);
          break;

        case 8:  // PARITY BIT
          if (bit != _bits_parity) {
            _log("P");
            // ignore it
          }
          break;

        default: // DATA BIT
          // Store and and schedule next sample.
          // LOG("%c", bit ? 'x' : '_');
          _bits_parity ^= bit;
          _bits_data |= (bit & 1) << _bits_count;
          break;
        }
        _bits_count++;
        _delay = _clock_div;
      }
      break;

    default:
      // NOT REACHED
      break;
    }
  }
}



