# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <math.h>
# define max_iter 1000

struct vectors {
	int n_features;
	int n_samples;
	double* coords;
};

struct dataspace {
	struct vectors* data_space;
	int* labels;
};


void check_if_data(char first, char second, int x, int y)
{
	if (x == 0 && y != 0)
	{
		printf("Check the first line.\n");
		exit(1);
	}
	else if (first == ' ' || first == '\n' || first == '.')
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

struct vectors* allocate_points(int* n_features, int* n_samples)
{
	struct vectors* points;
	points = malloc(sizeof(struct vectors));
	if (points == 0)
	{
		return 1;
	}
	points->n_features = *n_features;
	points->n_samples = *n_samples;
	points->coords = malloc(points->n_features * points->n_samples * sizeof(double));
	if (points->coords == 0) {
		free(points);
		return 1;
	}
	return points;
}

void destroy_vectors(struct vectors* some_vectors)
{
	free(some_vectors->coords);
	free(some_vectors);
}

struct dataspace* allocate_data(int* n_features, int* n_samples)
{
	struct dataspace* some_data;
	some_data = malloc(sizeof(struct dataspace));
	if (some_data == 0)
	{
		return 1;
	}
	some_data->data_space = allocate_points(n_features, n_samples);
	some_data->labels = malloc(some_data->data_space->n_samples*sizeof(int));
	if (some_data->labels == 0)
	{
		destroy_vectors(some_data->data_space);
		free(some_data);
		return 1;
	}
	return some_data;
}

void destroy_dataspace(struct dataspace* some_data)
{
	free(some_data->labels);
	destroy_vectors(some_data->data_space);
	free(some_data);
}

void get_dimensions(int* n_features, int* n_samples, FILE* given_data)
{
	int x = -1;
	int y = -1;
	int c = getc(given_data);
	while (c != EOF)
	{
		if (c == ' ')
		{
			x++;
		}
		else if (c == '\n')
		{
			x++;
			y++;
		}

		char b = c;
		c = getc(given_data);
		check_if_data(b, c, x, y);
	}
	*n_features = x / y;
	*n_samples = y;
	fseek(given_data, 0L, SEEK_SET);
}

void get_clusters_n(int* clusters_number, int* n_samples, FILE* given_data)
{
	if (fscanf(given_data, "%i", clusters_number) != 1)
	{
		printf("Check data file for typos.");
		exit(1);
	}
	if (*clusters_number > * n_samples)
	{
		printf("You can't have more clusters than samples.\n");
		exit(1);
	}
}

void load_the_data(struct dataspace* taken_data, FILE* given_data)
{
	for (int y = 0; y < taken_data->data_space->n_samples; y++)
	{
		for (int x = 0; x < taken_data->data_space->n_features; x++)
		{
			if (fscanf(given_data, "%lf", &taken_data->data_space->coords[y * taken_data->data_space->n_features + x]) != 1)
			{
				printf("Check data file for typos.");
				exit(1);
			}

		}
	}
}

void swap(int* a, int* b)
{
	int dummy = *a;
	*a = *b;
	*b = dummy;
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

void initialize_k_means(struct dataspace* taken_data, struct vectors* some_clusters)
{
	int* indices;
	indices = malloc(sizeof(int) * taken_data->data_space->n_samples);
	for (int i = 0; i < taken_data->data_space->n_samples; i++)
	{
		indices[i] = i;
	}
	fisher_yates(taken_data->data_space->n_samples, some_clusters->n_samples, indices);
	for (int j = 0; j < some_clusters->n_samples; j++)
	{
		for (int k = 0; k < some_clusters->n_features; k++)
		{
			some_clusters->coords[j * some_clusters->n_features + k] = taken_data->data_space->coords[indices[j] * taken_data->data_space->n_features + k];
		}
	}
	free(indices);
}

double Euclidean_distance(double* given_points, double* centres, int dimensions, int point_number, int centre_number) // calculates distance between two points
{
	double distance = 0;
	for (int i = 0; i < dimensions; i++)
	{
		double distance_for_axis = given_points[point_number * dimensions + i] - centres[centre_number * dimensions + i];
		distance = distance + distance_for_axis*distance_for_axis;
	}
	distance = sqrt(distance);
	return distance;
}

void label_points(struct dataspace* taken_data, struct vectors* some_clusters)
{
	double cur_dist;
	double min_dist;

	for (int j = 0; j < taken_data->data_space->n_samples; j++)
	{
		min_dist = Euclidean_distance(taken_data->data_space->coords, some_clusters->coords, taken_data->data_space->n_features, j, 0);
		taken_data->labels[j] = 0;
		for (int k = 0; k < some_clusters->n_samples; k++)
		{
			cur_dist = Euclidean_distance(taken_data->data_space->coords, some_clusters->coords, taken_data->data_space->n_features, j, k);
			if (cur_dist < min_dist)
			{
				taken_data->labels[j] = k;
				min_dist = cur_dist;
			}
		}
	}
}

void calculate_centroids(struct dataspace* taken_data, struct vectors* some_clusters)
{
	for (int cur_cent = 0; cur_cent < some_clusters->n_samples; cur_cent++)
	{
		for (int cur_dim = 0; cur_dim < taken_data->data_space->n_features; cur_dim++)
		{
			double sum_on_axis = 0;
			int members = 0;
			for (int cur_point = 0; cur_point < taken_data->data_space->n_samples; cur_point++)
			{
				if (taken_data->labels[cur_point] == cur_cent)
				{
					++members;
					sum_on_axis += taken_data->data_space->coords[cur_point * taken_data->data_space->n_features + cur_dim];
				}
				some_clusters->coords[cur_cent * some_clusters->n_features + cur_dim] = sum_on_axis / members;
			}
		}
	}
}

void kmeans_algorithm(struct dataspace* taken_data, struct vectors* some_clusters)
{
	label_points(taken_data, some_clusters); // labeling first time for already initialized centroids
	for (int i = 0; i < max_iter; i++)
	{
		calculate_centroids(taken_data, some_clusters); // recalculating centroids by means
		label_points(taken_data, some_clusters); // labeling
	}
}

void save_the_result(struct dataspace* taken_data, struct vectors* some_clusters)
{
	FILE* result;

	fopen_s(&result, "result.txt", "w");
	if (result == NULL)
	{
		perror("Creating the file failed.");
		return 1;
	}
	fprintf(result, "You decided to have %i clusters.\n", some_clusters->n_samples);

	fprintf(result, "This resulted in following set of labels (given in the same order as your data):\n");
	for (int i = 0; i < taken_data->data_space->n_samples; i++)
	{
		fprintf(result, "%i ", taken_data->labels[i]);
	}

	fprintf(result, "\nWhich stands true for following centroids (in this order):\n");
	for (int x = 0; x < some_clusters->n_samples; x++)
	{
		for (int y = 0; y < some_clusters->n_features; y++)
		{
			fprintf(result, "%lf ", some_clusters->coords[x * some_clusters->n_features + y]);
		}
		fprintf(result, "\n");
	}

	fclose(result);
	printf("Data saved in the file result.txt\n");
}

void free_all(struct dataspace* taken_data, struct vectors* some_clusters)
{
	destroy_vectors(some_clusters);
	destroy_dataspace(taken_data);
}

int main()
{
	int n_features; 
	int n_samples;
	int clusters_number;
	struct vectors* some_clusters;
	struct dataspace* taken_data;
	FILE* given_data;

	fopen_s(&given_data, "data.txt", "r");
	if (given_data == NULL)
	{
		perror("Opening the file failed.");
		return 1;
	}
	get_dimensions(&n_features, &n_samples, given_data);
	get_clusters_n(&clusters_number, &n_samples, given_data);
	some_clusters = allocate_points(&n_features, &clusters_number);
	taken_data = allocate_data(&n_features, &n_samples);
	load_the_data(taken_data, given_data);
	fclose(given_data);
	initialize_k_means(taken_data, some_clusters);
	kmeans_algorithm(taken_data, some_clusters);
	save_the_result(taken_data, some_clusters);
	free_all(taken_data, some_clusters);
	system("pause");
	return 0;
}
