#ifndef TMS_DATA_H
#define TMS_DATA_H

#define TB_NUM_SEGMENTS 6U
#define TB_CELLS_PER_SEGMENT 23U

// Shared interface of processed TMS temperature data. The data stored in these structs is the processed information from satellite boards.

typedef struct {
	float cell_temp_c[TB_CELLS_PER_SEGMENT];
} tb_segment_temps;

typedef struct {
	float segments[TB_NUM_SEGMENTS];
} tms_temperature_data;

#endif

