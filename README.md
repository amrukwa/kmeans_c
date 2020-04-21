# kmeans_c

[![Build status](https://ci.appveyor.com/api/projects/status/yj1sdcuf0883iaud?svg=true)](https://ci.appveyor.com/project/amrukwa/kmeans-c)
[![CodeFactor](https://www.codefactor.io/repository/github/amrukwa/kmeans_c/badge)](https://www.codefactor.io/repository/github/amrukwa/kmeans_c)

## K-means algorithm implementation in C


![image](https://user-images.githubusercontent.com/57860857/79414861-11ebf400-7fac-11ea-9898-f731298baeeb.png)
----

This program performs k-means clustering on given data stored in input file (the name of the file should be *data.txt* and creates new file *result.txt* with the outcome.

## Parameters

In this version the used distance metric is Euclidean distance. The number of iterations is defined in the code as 1000. The initialization method is MacQueen's random method.

## The data file

As mentioned above, the data to be clustered should be stored in *data.txt* (if you are using the executable file, otherwise you can change it in main()). The file must be in the same folder as the executable file. You can use as many samples and features as you wish - the program will take care of the dimensions.
In the first line you should place the number of clusters you wish to obtain and nothing more, otherwise program won't work.
In the following lines insert points coordinates, don't use commas. The delimiter is set to space. You can use exponential notation. Every line, including the last line, should be ended with enter.

## The outcome

The clustering parameters - chosen number of clusters, labels in the same order as given in data file and centroid of each cluster so that further use of the outcome is possible -  is stored in the output file *result.txt*. The file will appear in the same folder as executable version of the porgramme.

## The program

You can download the program [here](https://github.com/amrukwa/kmeans_c/releases).

The actual version of the program is marked as _Latest release_.
To download the code, click `Source code (zip)` or `Source code (tar.gz)` and save it on your computer.
To download the executable file, click `kmeans_in_c.exe` and save it on your computer. The output file storing the most vital information about performed clustering will be created in the same folder as saved `.exe` file. Remember to prepare the datafile as marked in **The data file**.

