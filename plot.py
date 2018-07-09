#!/usr/bin/env python3

import codecs
import cantools
import matplotlib.pyplot as plt

from argparse import ArgumentParser
from csv import reader

class Signal(object):
    def __init__(self, signal, database, data):
        self.__message  = signal.split('.')[0]
        self.__signal   = signal.split('.')[1]
        self.__database = database
        self.__format   = database.get_message_by_name(self.__message)
        self.__data     = self.getSignalData(data)

    def getSignalData(self, data):
        print("Filtering signal '{}' with ID={}".format(
            self.__signal,
            self.__format.frame_id
        ))

        timestamps = list()
        values = list()
        for message in data:
            # make sure the ID is the one we want
            frame_id = int(message[1])
            if not frame_id == self.__format.frame_id:
                continue

            # decode the bytestring
            bytedata = int(message[3]).to_bytes(8, 'little')

            # decode the CAN message
            msg = self.__database.decode_message(frame_id, bytedata)

            # get the timestamp and the value of the signal
            timestamps.append(int(message[0]))
            values.append(msg[self.__signal])
        
        return (timestamps, values)

    def plot(self, figure):
        (x, y) = self.__data
        plt.plot(x, y, label=self.__signal, figure=figure)

    def getSignalName(self):
        return self.__signal

def singlePlot(signals, save=None):
    figure = plt.figure()
    plt.title("CAN messages", figure=figure)
    plt.xlabel("Timestamps (ms)", figure=figure)
    plt.ylabel("Values", figure=figure)
    for signal in signals:
        signal.plot(figure)
    ax = figure.gca()
    ax.legend()

    if save is not None:
        plt.savefig(save)
    else:
        plt.show()

def splittedPlots(signals, save=None):
    for signal in signals:
        figure = plt.figure()
        plt.title("CAN messages", figure=figure)
        plt.xlabel("Timestamps (ms)", figure=figure)
        plt.ylabel(signal.getSignalName(), figure=figure)
        signal.plot(figure)

        if save is not None:
            filename = "{}_{}.{}".format(
                save.split('.')[0],
                signal.getSignalName(),
                save.split('.')[1]
            )
            plt.savefig(filename)

    if save is None:
        plt.show()

if __name__ == '__main__':
    # create the argument parser
    parser = ArgumentParser(
        description='Script used for interpreting data from the DAQ.'
    )

    parser.add_argument(
        '--database',
        type=str,
        help='the path to the CAN database',
        metavar='DB',
        required=True,
    )

    parser.add_argument(
        '--data',
        type=str,
        help='the path to the CSV file containing the data ',
        metavar='CSV',
        required=True,
    )

    parser.add_argument(
        '--signal',
        type=str,
        help='a signal to plot',
        metavar='MSG.SIG',
        required=True,
        action='append',
    )

    parser.add_argument(
        '--save',
        type=str,
        help='the filename of the save plot',
        metavar='FILENAME',
        required=False,
    )

    parser.add_argument(
        '--debug',
        help='useful for debugging this scripts',
        required=False,
        action='store_true',
    )

    parser.add_argument(
        '--split',
        help='create multiple plots rather than a single one',
        required=False,
        action='store_true',
    )

    # parse the arguments
    args = parser.parse_args()

    # read the CSV file
    data = list()
    try:
        with open(args.data, 'r') as csvfile:
            csvreader = reader(csvfile, delimiter=',')
            for row in csvreader:
                if len(row) != 4:
                    print("Malformed CSV line '{}'".format(row))
                    continue
                data.append(row)
    except FileNotFoundError:
        print("The CSV file '{}' couldn't be read.".format(args.data))
        exit(1)

    # load the database
    try:
        database = cantools.db.load_file(args.database)
    except FileNotFoundError:
        print("Database file {}` couldn't be read.".format(args.database))
        exit(1)

    # print the database if debug is activated
    if args.debug:
        for message in database.messages:
            print(message, message.signals)

    # create the signals data
    signals = list()
    for signal in args.signal:
        try:
            signals.append(Signal(signal, database, data))
        except KeyError:
            print("Signal '{}' isn't in the database.".format(signal))

    # save or show the figures
    if args.split:
        splittedPlots(signals, save=args.save)
    else:
        singlePlot(signals, save=args.save)
