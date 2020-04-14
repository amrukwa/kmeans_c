# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <math.h>
# define max_iter 1000

typedef struct {
	int n_features;
	int n_samples;
	double * data; // 2D array containing n_samples vectors of n_features dimensions
	int * labels; // 1D array containing label for each sample
} data_space;

typedef struct {
	int n_clusters;
	double * centroids; // 2D array containing n_clusters vectors of n_features dimensions
}cluster_params;

void check_if_data(char first, char second)
{
	if (first == ' ' || first == '\n' || first == '.')
	{
		if (second == ' ' || second == '\n' || second == '.')
		{
			printf("Check whitespace characters and dots.\n");
			exit(1);
		}
	}
	else if (first == ',')
	{
		printf("Change commas into dots\n");
		exit(1);
	}
	else if (second == EOF)
	{
		if (first != '\n')
		{
			printf("No newline at the end of file.");
			exit(1);
		}
	}
}

void get_dimensions(data_space * dimensions, FILE * given_data)
{
	int x = 0;
	int y = 0;
	int c = getc(given_data);
	while (c != EOF)
	{
		if (c == ' ')
		{
			x++;
		}
		else if(c == '\n')
		{
			x++;
			y++;
		}
		
		char b = c;
		c = getc(given_data);
		check_if_data(b, c);
	}
	dimensions->n_features = x / y;
	dimensions->n_samples = y;
	fseek(given_data, 0L, SEEK_SET);
}

void get_clusters_n(cluster_params* some_clusters, data_space* dimensions)
{
	printf("Type the desired number of centroids.\n");
	scanf("%i", &some_clusters->n_clusters);
	if (some_clusters->n_clusters > dimensions->n_samples)
	{
		printf("You can't have more clusters than samples.\n");
		exit(1);
	}
}

void allocate_point_memory(data_space* dimensions, cluster_params* some_clusters) // add checking if memory was allocated
{
	dimensions->data = (double*)malloc(dimensions->n_samples * dimensions->n_features * sizeof(double));
	some_clusters->centroids = (double*)malloc(some_clusters->n_clusters * (dimensions->n_features) * sizeof(double));
	dimensions->labels = (int*)calloc(dimensions->n_samples, sizeof(int)); // this way we can skip one iteration later on in first labeling and prevent reseting labels at each call
}

void load_the_data(data_space* dimensions, FILE* given_data)
{
	for (int y = 0; y < dimensions->n_samples; y++)
	{
		for (int x = 0; x < dimensions->n_features; x++)
		{
			if (fscanf(given_data, "%lf", &dimensions->data[y * dimensions->n_features + x]) != 1)
			{
				printf("Check data file for typos.");
				exit(1);
			}
		}
	}
}

void print_to_check(cluster_params* some_clusters, data_space* dimensions)
{
	for (int x = 0; x < some_clusters->n_clusters; x++)
	{
		for (int y = 0; y < dimensions->n_features; y++)
		{
			printf("%lf", some_clusters->centroids[x * dimensions->n_features + y]);
			printf(" ");
		}
		printf("\n");
	}
}

void swap(int* a, int* b)
{
	int dummy = *a;
	*a = *b;
	*b = dummy;
}

void fisher_yates(int initial_dimensions, int desired_dimensions, int* indices);

void initialize_k_means(data_space* dimensions, cluster_params* some_clusters)
{
	int* indices;
	indices = (int*)malloc(sizeof(int) * dimensions->n_samples);
	for (int i = 0; i < dimensions->n_samples; i++)
	{
		indices[i] = i;
	}
	fisher_yates(dimensions->n_samples, some_clusters->n_clusters, indices);
	for (int j = 0; j < some_clusters->n_clusters; j++)
	{
		for (int k = 0; k < dimensions->n_features; k++)
		{
			some_clusters->centroids[j * dimensions->n_features + k] = dimensions->data[indices[j] * dimensions->n_features + k];
		}
	}	
}

void fisher_yates(int initial_dimensions, int desired_dimensions, int* indices)
{
	srand(time(NULL));
	for (int i = 0; i < desired_dimensions; i++) // for the sake of optimalization it should be sufficient
	{
		int a = rand() % initial_dimensions; // index between 1st and last data sample
		swap(&indices[i], &indices[a]);
	}
}

double Euclidean_distance(double* given_points, double* centres, int dimensions, int point_number, int centre_number) // calculates distance between two points
{
	double distance = 0;
	for (int i = 0; i < dimensions; i++)
	{
		double distance_for_axis = given_points[point_number * dimensions + i] - centres[centre_number * dimensions + i];
		distance = distance + pow(distance_for_axis, 2);
	}
	distance = pow(distance, 0.5);
	return distance;
}

void label_points(cluster_params* some_clusters, data_space* dimensions)
{
	double cur_dist;
	double min_dist;

	for (int j = 0; j < dimensions->n_samples; j++)
	{
		min_dist = Euclidean_distance(dimensions->data, some_clusters->centroids, dimensions->n_features, j, 0);
		dimensions->labels[j] = 0;
		for (int k = 0; k < some_clusters->n_clusters; k++)
		{
			cur_dist = Euclidean_distance(dimensions->data, some_clusters->centroids, dimensions->n_features, j, k);
			if (cur_dist < min_dist)
			{
				dimensions->labels[j] = k;
				min_dist = cur_dist;
			}
		}
	}
}

void calculate_centroids(cluster_params* some_clusters, data_space* dimensions) 
{
	for (int cur_cent = 0; cur_cent < some_clusters->n_clusters; cur_cent++)
	{
		for (int cur_dim = 0; cur_dim < dimensions->n_features; cur_dim++)
		{
			double sum_on_axis = 0;
			int members = 0;
			for (int cur_point = 0; cur_point < dimensions->n_samples; cur_point++)
			{
				if (dimensions->labels[cur_point] == cur_cent)
				{
					members= members+1;
					sum_on_axis = sum_on_axis + dimensions->data[cur_point * dimensions->n_features + cur_dim];
				}
				some_clusters->centroids[cur_cent * dimensions->n_features + cur_dim] = sum_on_axis / members;
			}
		}
	}
}

void kmeans_algorithm(cluster_params* some_clusters, data_space* dimensions)
{
	label_points(some_clusters, dimensions); // labeling first time for already initialized centroids
	for (int i = 0; i < max_iter; i++)
	{
		calculate_centroids(some_clusters, dimensions); // recalculating centroids by means
		label_points(some_clusters, dimensions); // labeling
	}
}

void free_all(cluster_params* some_clusters, data_space* dimensions)
{
	free(dimensions->data);
	free(dimensions->labels);
	free(some_clusters->centroids);
}

int main()
{
	data_space dimensions;
	cluster_params some_clusters;
	FILE* given_data;
	fopen_s(&given_data, "data.txt", "r");
	get_dimensions(&dimensions, given_data);
	get_clusters_n(&some_clusters, &dimensions);
	allocate_point_memory(&dimensions, &some_clusters);
	load_the_data(&dimensions, given_data);
    fclose(given_data);
	initialize_k_means(&dimensions, &some_clusters);
	print_to_check(&some_clusters, &dimensions);
	kmeans_algorithm(&some_clusters, &dimensions);
	print_to_check(&some_clusters, &dimensions); //remember to transform it into save_the_result
	free_all(&some_clusters, &dimensions);
	return 0;
}