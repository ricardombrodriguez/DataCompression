import os
import sys
import pandas as pd
import matplotlib.pyplot as plt

if len(sys.argv) != 2:
    print("provide directory of runs.")
    print("\tpython analysis.py DIR")
    exit(1)


def load_runs(DIR):
    data = {
        'file': [],
        'k': [],
        'prob': [],
        'alpha': [],
        'bits': []
    }

    for run in os.listdir(DIR):
        with open(f"{DIR}/{run}", 'r') as r:
            lines = r.readlines()
        
        for l in lines:
            key, val = [item.strip() for item in l.split(":")]

            if key == "k":
                val = int(val)
            elif key == "file":
                try:
                    val = str(val.split("/")[-1])
                except:
                    val = str(val)
            else:
                val = float(val)

            data[key].append(val)

    return pd.DataFrame(data)


def line_chart(data, filename, x, y):
    file_data = data[data["file"] == filename]

    plot = file_data.plot.scatter(x=x, y=y)
    plot.set_title(filename)
    plot.set_xlabel(x)
    plot.set_ylabel(y)
    plt.show()
    

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("provide directory of runs.")
        print("\tpython analysis.py DIR")
        exit(1)
    DIR = sys.argv[1]

    frame = load_runs(DIR)

    line_chart(frame, "teste.txt", "k", "bits")
    line_chart(frame, "teste.txt", "alpha", "bits")




        