# Handwritten-Digit-Recognition-using-KNN-and-Ridge-Regression
The csv files contain original pixel representation of the (28 x 28) images of the MNIST dataset

Steps to run:

KNN

1)make knn

2)./knn train-file validation-file test-file output-file

Ridge regression

1)make regression

2)./regression train-file validation-file test-file output-file weights_file

nn_regression : It is for learning non-negative weights. With these weights we can construct the image. These plots will help you identify which pixels are discriminatory for the class

1)make nn_regression

2)./nn_regression train-file validation-file test-file output-file weights_file
