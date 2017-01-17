// CS 61C Fall 2015 Project 4

// include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif

// include OpenMP
#if !defined(_MSC_VER)
#include <pthread.h>
#endif
#include <omp.h>
#include <stdio.h>

#include "calcDepthOptimized.h"
#include "calcDepthNaive.h"

/* DO NOT CHANGE ANYTHING ABOVE THIS LINE. */

#define MAX(x, y) ((x > y) ? x: y)
#define MIN(x, y) ((x < y) ? x: y)

float displacementOptimized(int dx, int dy)
{
	return sqrt(dx * dx + dy * dy);
}

void calcDepthOptimized(float *depth, float *left, float *right, int imageWidth, int imageHeight, int featureWidth, int featureHeight, int maximumDisplacement)
{
	/* The two outer for loops iterate through each pixel */
	//int tail_index = (2 * featureHeight + 1) % 4;

	#pragma omp parallel for collapse(2)
	for (int x = 0; x < imageWidth; x++)
	//for (int y = 0; y < imageHeight; y++)
	{
			for (int y = 0; y < imageHeight; y++)
			//for (int x = 0; x < imageWidth; x++)
			{	
				/* Set the depth to 0 if looking at edge of the image where a feature box cannot fit. */
				if ((y < featureHeight) || (y >= imageHeight - featureHeight) || (x < featureWidth) || (x >= imageWidth - featureWidth))
				{
					depth[y * imageWidth + x] = 0;
					continue;
				}

				float minimumSquaredDifference = -1;
				int minimumDy = 0;
				int minimumDx = 0;

				/* Iterate through all feature boxes that fit inside the maximum displacement box. 
				   centered around the current pixel. */
				for (int dy = -maximumDisplacement; dy <= maximumDisplacement; dy++)
				{
					for (int dx = -maximumDisplacement; dx <= maximumDisplacement; dx++)
					{
						/* Skip feature boxes that dont fit in the displacement box. */
						if (y + dy - featureHeight < 0 || y + dy + featureHeight >= imageHeight || x + dx - featureWidth < 0 || x + dx + featureWidth >= imageWidth)
						{
							continue;
						}

						float squaredDifference = 0;
						//int tail_index = (2 * featureHeight + 1) % 4;
						//int end_point = featureHeight - tail_index + 1;

						/* Sum the squared difference within a box of +/- featureHeight and +/- featureWidth. */
						//for (int boxX = -featureWidth; boxX <= featureWidth; boxX++)
						for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
						{
							int leftY = (y + boxY) * imageWidth;
							int rightY = (y + dy + boxY) * imageWidth;
							//int leftX = x + boxX;
							//int rightX = x + dx + boxX;
							int left_index, right_index, leftX, rightX;
							//int left_index, right_index, leftY, rightY;
							float difference;
							int tail;
							__m128 summation_vec = _mm_setzero_ps();
							//int tail_index;

							if (featureWidth < 8) {
								for (int boxX = -featureWidth; boxX <= featureWidth - 3; boxX += 4)
								//for (int boxY = -featureHeight; boxY <= featureHeight - 3; boxY += 4)
								{
									leftX = x + boxX;
									//leftY = (y + boxY) * imageWidth;
									rightX = x + dx + boxX;
									//rightY = (y + dy + boxY) * imageWidth;
									left_index = leftX + leftY;
									right_index = rightX + rightY;
									__m128 left_vec = _mm_loadu_ps(left + left_index);
									__m128 right_vec = _mm_loadu_ps(right + right_index);
									__m128 diff_vec = _mm_sub_ps(left_vec, right_vec);
									__m128 square_vec = _mm_mul_ps(diff_vec, diff_vec);							
									summation_vec = _mm_add_ps(summation_vec, square_vec);
									tail = boxX + 4;
									//tail = boxY + 4;
								}
							} else {
								for (int boxX = -featureWidth; boxX <= featureWidth - 15; boxX += 16)
								//for (int boxY = -featureHeight; boxY <= featureHeight - 3; boxY += 4)
								{
									leftX = x + boxX;
									//leftY = (y + boxY) * imageWidth;
									rightX = x + dx + boxX;
									//rightY = (y + dy + boxY) * imageWidth;
									left_index = leftX + leftY;
									right_index = rightX + rightY;
									__m128 left_vec = _mm_loadu_ps(left + left_index);
									__m128 right_vec = _mm_loadu_ps(right + right_index);
									__m128 diff_vec = _mm_sub_ps(left_vec, right_vec);
									__m128 square_vec = _mm_mul_ps(diff_vec, diff_vec);							
									summation_vec = _mm_add_ps(summation_vec, square_vec);
									tail = boxX + 4;
									//tail = boxY + 4;
								}
							}
								//float difference = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
								//float difference = left[leftY + leftX] - right[rightY + rightX];
							// tail case
							for (int i = tail; i <= featureWidth; i++) {
							//for (int i = tail; i <= featureHeight; i++) {
								difference = left[leftY + x + i] - right[rightY + x + i + dx];
								//difference = left[leftX + (y + i) * imageWidth] - right[rightX + (y + i + dy) * imageWidth];
								squaredDifference += difference * difference;
							}
								//squaredDifference += difference * difference;
							squaredDifference += (summation_vec[0] + summation_vec[1] + summation_vec[2] + summation_vec[3]);
							
						}

						/* 
						Check if you need to update minimum square difference. 
						This is when either it has not been set yet, the current
						squared displacement is equal to the min and but the new
						displacement is less, or the current squared difference
						is less than the min square difference.
						*/
						if ((minimumSquaredDifference == -1) || ((minimumSquaredDifference == squaredDifference) && (displacementNaive(dx, dy) < displacementNaive(minimumDx, minimumDy))) || (minimumSquaredDifference > squaredDifference))
						{
							minimumSquaredDifference = squaredDifference;
							minimumDx = dx;
							minimumDy = dy;
						}
					}
				}

				/* 
				Set the value in the depth map. 
				If max displacement is equal to 0, the depth value is just 0.
				*/
				if (minimumSquaredDifference != -1)
				{
					if (maximumDisplacement == 0)
					{
						depth[y * imageWidth + x] = 0;
					}
					else
					{
						depth[y * imageWidth + x] = displacementNaive(minimumDx, minimumDy);
					}
				}
				else
				{
					depth[y * imageWidth + x] = 0;
				}
			}

	}
}