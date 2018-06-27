import numpy as np
import time
from pylabcontrol.core import Script, Parameter
from b26_toolkit.instruments import B26KDC001x, B26KDC001z, B26KDC001y
from b26_toolkit.scripts.find_nv import FindNV
from b26_toolkit.scripts.daq_read_counter import Daq_Read_Counter
from b26_toolkit.plotting.plots_1d import plot_counts_vs_pos, update_counts_vs_pos
from collections import deque
import scipy as sp
from b26_toolkit.plotting.plots_2d import plot_fluorescence_pos, update_fluorescence

class ServoScan(Script):
    """
    ServoScan sweeps the position of the automatic translation stages, in 1D scans, and records NV fluorescence
    using Daq_Read_Counter, at each point in the scan.

    We use the NV fluorescence to align a magnetic bias field to the NV axis, generated by a permanent magnet.

    ER 20180606

    """

    _DEFAULT_SETTINGS = [
        Parameter('track_n_pts', 10, int, 'track every N points'),
        Parameter('scan_axis', 'x', ['x', 'y', 'z'], 'axis to scan on'),
        Parameter('num_points', 100, int, 'number of points in the scan'),
        Parameter('min_pos', 0., float, 'minimum position of scan (mm)'),
        Parameter('max_pos', 5., float, 'maximum position of scan (mm)'),
        Parameter('time_per_pt', 0.5, float, 'time to wait at each point (s)')
        ]

    _INSTRUMENTS = {'XServo': B26KDC001x, 'YServo': B26KDC001y, 'ZServo': B26KDC001z}

    _SCRIPTS = {'find_nv': FindNV, 'daq_read_counter': Daq_Read_Counter}

    def _get_instr(self):
        """

        Assigns an instrument relevant to the 1D scan axis.

        """
        if self.settings['scan_axis'] == 'x':
            return self.instruments['XServo']['instance']
        elif self.settings['scan_axis'] == 'y':
            return self.instruments['YServo']['instance']
        elif self.settings['scan_axis'] == 'z':
            return self.instruments['ZServo']['instance']

    def _get_scan_positions(self, verbose=True):
        '''
        Returns an array of points to go to in the 1D scan.
        '''
        if self.settings['scan_axis'] in ['x', 'y', 'z']:
            scan_pos = [np.linspace(self.settings['min_pos'], self.settings['max_pos'], self.settings['num_points'])]

            if verbose:
                print('values for the scan are (in mm):', scan_pos)

            return scan_pos
        else:
            NotImplementedError('multiple dimensional scans not yet implemented')

    def gaussian(self, x, noise, amp, center, width):
        return (noise + amp * np.exp(-1.0 * (np.square((x - center)) / (2 * (width ** 2)))))

    def fit_gaussian(self):
        noise_guess = np.min(self.data['counts'])
        amplitude_guess = np.max(self.data['counts']) - noise_guess
        center_guess = np.mean(self.data['positions'])
        width_guess = np.std(self.data['positions'])

        p2 = [noise_guess, amplitude_guess, center_guess, width_guess]

        pos_of_max = None

        print('trying to fit the gaussian!')

        try:
            p2, success = sp.optimize.curve_fit(self.gaussian, self.data['positions'],
                                                self.data['counts'], p0=p2,
                                                bounds=([0, [np.inf, np.inf, 100., 100.]]), max_nfev=2000)

            print('p2', p2)
            print('success', success)

            pos_of_max = p2[2]

            self.log('Found fit parameters: ' + str(p2))
        except(ValueError, RuntimeError):
            self.log('Could not converge to fit parameters')

        return pos_of_max, p2

    def _function(self):

        # get the relevant instrument.
        scan_instr = self._get_instr()

        # get positions for the scan.
        scan_pos = self._get_scan_positions()

        # data structure
        self.data = {'counts': deque()}
        self.positions = {'positions': deque()}

        # place for fit parameters
        self.data['fit_parameters'] = [0, 0, 0, 0]
        self.data['positions'] = scan_pos

        # loop over scan positions and call the scripts
        for index in range(0, self.settings['num_points']):
            if self._abort:
                break

            new_pos = float(scan_pos[0][index])
            scan_instr.settings['position'] = new_pos # update the position setting of the instrument
            scan_instr.set_position() # actually move the instrument to that location. If this is not within the safety
                                      # limits of the instruments, it will not actually move and say so in the log

            # run daq_read_counter or the relevant script to get fluorescence
            self.scripts['daq_read_counter'].run()
            time.sleep(self.settings['time_per_pt'])
            self.scripts['daq_read_counter'].stop()

            # add to output structures which will be plotted
            data = self.scripts['daq_read_counter'].data['counts']
            self.data['counts'].append(np.mean(data))
            self.positions['positions'].append(new_pos)

            self.progress = index*100./self.settings['num_points']
            self.updateProgress.emit(int(self.progress))


            # track to the NV
            if index > 0 and index % self.settings['track_n_pts'] == 0:
                self.scripts['find_nv'].run()

        # clean up data, as in daq_read_counter
        self.data['counts'] = list(self.data['counts'])

        # attempt to fit
        pos_of_max, self.data['fit_parameters'] = self.fit_gaussian()
        if pos_of_max:
            print('found maximum fluorescence position at: ', pos_of_max, ' mm')



    def plot(self, figure_list):
        super(ServoScan, self).plot([figure_list[0]])

    def _plot(self, axes_list, data = None):
        # COMMENT_ME

        if data is None:
            data = self.data

        if data:
            plot_counts_vs_pos(axes_list[0], data['counts'], self.positions['positions'])

        # if the fit has finished plot the result
        if not (np.array_equal(data['fit_parameters'], [0, 0, 0, 0])):
            axes_list[0].plot(data['positions'],
                            self.gaussian(data['positions'], *self.data['fit_parameters']), 'k')

    def _update_plot(self, axes_list):
        update_counts_vs_pos(axes_list[0], self.data['counts'], self.positions['positions'])

class ServoScan_2D(Script):
    """
    ServoScan_2D sweeps the position of the automatic translation stages, in 2D scans, and records NV fluorescence
    using Daq_Read_Counter, at each point in the scan.

    We use the NV fluorescence to align a magnetic bias field to the NV axis, generated by a permanent magnet.

    ER 20180618

    """

    _DEFAULT_SETTINGS = [
        Parameter('track_n_pts', 10, int, 'track every N points'),
        Parameter('outer_loop',
                  [
                      Parameter('scan_axis', 'x', ['x', 'y', 'z'], 'outer loop axis to scan on'),
                      Parameter('min_pos', 0., float, 'minimum position of scan (mm)'),
                      Parameter('max_pos', 5., float, 'maximum position of scan (mm)'),
                      Parameter('num_points', 100, int, 'number of points in the outer loop')
                  ]),
        Parameter('inner_loop',
                  [
                      Parameter('scan_axis', 'y', ['x', 'y', 'z'], 'inner loop axis to scan on'),
                      Parameter('min_pos', 0., float, 'minimum position of scan (mm)'),
                      Parameter('max_pos', 5., float, 'maximum position of scan (mm)'),
                      Parameter('num_points', 100, int, 'number of points in the inner loop')
                  ]),
        Parameter('time_per_pt', 0.5, float, 'time to wait at each point (s)')
    ]

    _INSTRUMENTS = {'XServo': B26KDC001x, 'YServo': B26KDC001y, 'ZServo': B26KDC001z}

    _SCRIPTS = {'find_nv': FindNV, 'daq_read_counter': Daq_Read_Counter}

    def _get_instr(self):
        """

        Assigns an instrument relevant to the 1D scan axis.

        """
        if self.settings['outer_loop']['scan_axis'] == 'x':
            outer_instr = self.instruments['XServo']['instance']
        elif self.settings['outer_loop']['scan_axis'] == 'y':
            outer_instr = self.instruments['YServo']['instance']
        elif self.settings['outer_loop']['scan_axis'] == 'z':
            outer_instr = self.instruments['ZServo']['instance']

        if self.settings['inner_loop']['scan_axis'] == 'x':
            inner_instr = self.instruments['XServo']['instance']
        elif self.settings['inner_loop']['scan_axis'] == 'y':
            inner_instr = self.instruments['YServo']['instance']
        elif self.settings['inner_loop']['scan_axis'] == 'z':
            inner_instr = self.instruments['ZServo']['instance']

        if outer_instr == inner_instr:
            AttributeError('pick two different axis for the scan please!')

        return outer_instr, inner_instr

    def _get_scan_positions(self, verbose=True):
        '''
        Returns an array of points to go to in the 2D scan.
        '''

        min_out = self.settings['outer_loop']['min_pos']
        max_out = self.settings['outer_loop']['max_pos']
        num_out = self.settings['outer_loop']['num_points']

        min_in = self.settings['inner_loop']['min_pos']
        max_in = self.settings['inner_loop']['max_pos']
        num_in = self.settings['inner_loop']['num_points']

        scan_pos_outer = np.linspace(min_out, max_out, num_out)
        scan_pos_inner = np.linspace(min_in, max_in, num_in)

        return scan_pos_outer, scan_pos_inner

    def gaussian(self, x, noise, amp, center, width):
        return (noise + amp * np.exp(-1.0 * (np.square((x - center)) / (2 * (width ** 2)))))

    def fit_gaussian(self):
        noise_guess = np.min(self.data['counts'])
        amplitude_guess = np.max(self.data['counts']) - noise_guess
        center_guess = np.mean(self.data['positions'])
        width_guess = np.std(self.data['positions'])

        p2 = [noise_guess, amplitude_guess, center_guess, width_guess]

        pos_of_max = None

        print('trying to fit the gaussian!')

        try:
            p2, success = sp.optimize.curve_fit(self.gaussian, self.data['positions'],
                                                self.data['counts'], p0=p2,
                                                bounds=([0, [np.inf, np.inf, 100., 100.]]), max_nfev=2000)

            print('p2', p2)
            print('success', success)

            pos_of_max = p2[2]

            self.log('Found fit parameters: ' + str(p2))
        except(ValueError, RuntimeError):
            self.log('Could not converge to fit parameters')

        return pos_of_max, p2

    def _function(self):

        # get the relevant instrument.
        outer_instr, inner_instr = self._get_instr()
        print('outer_instr', outer_instr)
        print('inner_instr', inner_instr)

        # get positions for the scan.
        scan_pos_outer, scan_pos_inner = self._get_scan_positions()

        print('scan_pos_outer', scan_pos_outer)
        print('scan_pos_inner', scan_pos_inner)
        # data structure
       # self.data = {'counts': deque()}
        self.data['counts'] = [[0. for x in range(self.settings['outer_loop']['num_points'])] for y in range(self.settings['inner_loop']['num_points'])]

        # place for positions
        self.data['positions_outer'] = scan_pos_outer
        self.data['positions_inner'] = scan_pos_inner

        tot_index = 0

        # loop over scan positions and call the scripts
        for index_out in range(0, self.settings['outer_loop']['num_points']):
            for index_in in range(0, self.settings['inner_loop']['num_points']):

                if self._abort:
                    break

                new_pos = [float(scan_pos_outer[int(index_out)]), float(scan_pos_inner[int(index_in)])]
                self.log('new pos: ' + str(new_pos) + ' mm')
                self.log('set outer instr: '+ str(outer_instr) + ' to ' + str(new_pos[0]) + ' mm')
                self.log('set inner instr: '+ str(inner_instr) + ' to ' + str(new_pos[1]) + ' mm')

                outer_instr.settings['position'] = new_pos[0]  # update the position setting of the instrument
                inner_instr.settings['position'] = new_pos[1]
                outer_instr.set_position()  # actually move the instrument to that location. If this is not within the safety
                inner_instr.set_position()  # limits of the instruments, it will not actually move and say so in the log

                # run daq_read_counter or the relevant script to get fluorescence
                self.scripts['daq_read_counter'].run()
                time.sleep(self.settings['time_per_pt'])
                self.scripts['daq_read_counter'].stop()

                # add to output structures which will be plotted
                data = self.scripts['daq_read_counter'].data['counts']
                self.data['counts'][index_in][index_out] = np.mean(data)

                self.progress = tot_index * 100. / (self.settings['inner_loop']['num_points']*self.settings['outer_loop']['num_points'])
                self.updateProgress.emit(int(self.progress))

                # track to the NV
                if tot_index > 0 and tot_index % self.settings['track_n_pts'] == 0:
                    self.scripts['find_nv'].run()

                tot_index = tot_index + 1

        print('self.data[counts]', self.data['counts'])

    def plot(self, figure_list):
        super(ServoScan_2D, self).plot([figure_list[0]])

    def _plot(self, axes_list, data=None):
        # COMMENT_ME

        extent = [self.settings['inner_loop']['min_pos'], self.settings['inner_loop']['max_pos'], self.settings['outer_loop']['min_pos'], self.settings['outer_loop']['max_pos']]

        if data is None:
            data = self.data

        if data:
            plot_fluorescence_pos(data['counts'], extent, axes_list[0])

    def _update_plot(self, axes_list):
        update_fluorescence(self.data['counts'], axes_list[0])


if __name__ == '__main__':
    succeeded, failed, _ = Script.load_and_append({'ServoScan': ServoScan})

    print(succeeded)