
void row_worker(float data[], int l) {
	int row_length = l, i;
	float **sheet;
	float *ret;

	ret = xmalloc(row_length * sizeof(float));

	sheet = xmalloc(3 * sizeof(float *));

	for (i = 0; i < 3; i++)
		sheet[i] = xmalloc(row_length * sizeof(float));


	for (i = 0; i < row_length; i++) {
		sheet[0][i] = data[i];
		sheet[1][i] = data[row_length + i];
		sheet[2][i] = data[row_length * 2 + i];
	}

	ret[0] = 0;
	ret[row_length - 1] = 0;

	for (i = 1; i < row_length - 1; i++)
		ret[i] = (sheet[0][i] + sheet[2][i] + sheet[1][i-1] + sheet[1][i] + sheet[1][i+i]) / 5.0;


	/* send ret */

	free(ret);

	for (i = 0; i < 3; i++)
		free(sheet[i]);

	free(sheet);
}
