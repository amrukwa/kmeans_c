# include <stdio.h>
# include <stdlib.h>

typedef struct {
	int n_features;
	int n_samples;
	float * data;
} data_space;

typedef struct {

	int n_clusters;
	float * centroids;
}cluster_params;

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
		c = getc(given_data);
	}
	dimensions->n_features = x / y;
	dimensions->n_samples = y;
	printf("%i", dimensions->n_features);
	printf("%i", dimensions->n_samples);
	fseek(given_data, 0L, SEEK_SET);
}

void get_clusters_n(cluster_params* some_clusters)
{
	printf("Type the desired number of centroids.\n");
	scanf("%i", &some_clusters->n_clusters);
}

void initialize_point_memory(data_space* dimensions, cluster_params* some_clusters) // add checking if memory was allocated
{
	dimensions->data = (float*)malloc(dimensions->n_samples * dimensions->n_features * sizeof(float));
	some_clusters->centroids = (float*)malloc(some_clusters->n_clusters * (dimensions->n_features) * sizeof(float));
}

int main()
{
	data_space dimensions;
	cluster_params some_clusters;
	FILE* given_data;
	fopen_s(&given_data, "data.txt", "r");
	get_dimensions(&dimensions, given_data);
	get_clusters_n(&some_clusters);
	initialize_point_memory(&dimensions, &some_clusters);
	//load the data into data_space


	fclose(given_data);
	free(dimensions.data);
	free(some_clusters.centroids);
	return 0;
}