# ECE 459 Assignment 4 90th Percentile Script
# Credit: Kevin Yang ( k46yang@edu.uwaterloo.ca )
# with additional credits to Google & Stack Overflow

import numpy as np
import csv
from glob import glob

filenames = glob("results.csv")
# results_{timestamp}_n={n}_a={a}_j={j}_l={l}_m={m}_b={b}_g={g}_.csv

output_file_name = "experimental_results.csv"

with open(output_file_name, "w+") as output_csv:
    writer = csv.writer(output_csv, delimiter=",")
    writer.writerow(['n', 'a', 'j', 'l', 'm', 'b', 'g', 'p90'])

    for filename in filenames:
        attributes = list()
        filename_metadata = filename.split("_")[2:9]
        for attr in filename_metadata:
            attributes.append(int(attr.split("=")[1]))

        with open(filename) as csvfile:
            reader = csv.reader(csvfile, delimiter=",")
            arr = list()
            for row in reader:
                arr.append(float(row[4]))
            percentile_90 = np.percentile(np.array(arr), 90)
            writer.writerow(attributes + [percentile_90])
