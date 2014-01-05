#ifndef LOOPFILTER_HH
#define LOOPFILTER_HH

#include <cstdint>

#include "optional.hh"
#include "modemv_data.hh"
#include "raster.hh"

struct KeyFrameHeader;
struct UpdateSegmentation;

enum class LoopFilterType : char { Normal, Simple, NoFilter };

struct SegmentFilterAdjustment
{
  bool absolute { false };
  int8_t value { 0 };

  SegmentFilterAdjustment() {}

  SegmentFilterAdjustment( const uint8_t segment_id,
			   const Optional< UpdateSegmentation > & update_segmentation );

  void update( const uint8_t segment_id,
	       const Optional< UpdateSegmentation > & update_segmentation );
};

struct FilterParameters
{
  LoopFilterType type;
  int filter_level; /* don't clamp until ready to make the filter */
  uint8_t sharpness_level;

  FilterParameters( const bool use_simple_filter,
		    const uint8_t s_filter_level,
		    const uint8_t s_sharpness_level,
		    const SegmentFilterAdjustment & segment_adjustment = SegmentFilterAdjustment() );

  void adjust( const SafeArray< int8_t, num_reference_frames > & ref_adjustments,
	       const SafeArray< int8_t, 4 > & mode_adjustments,
	       const reference_frame macroblock_reference_frame,
	       const mbmode macroblock_y_mode );
};

class SimpleLoopFilter
{
private:
  uint8_t filter_level_;
  uint8_t interior_limit_;
  uint8_t macroblock_edge_limit_;
  uint8_t subblock_edge_limit_;

public:
  SimpleLoopFilter( const FilterParameters & params );

  uint8_t filter_level( void ) const { return filter_level_; }
  uint8_t interior_limit( void ) const { return interior_limit_; }
  uint8_t macroblock_edge_limit( void ) const { return macroblock_edge_limit_; }
  uint8_t subblock_edge_limit( void ) const { return subblock_edge_limit_; }

  void filter( Raster::Macroblock & raster, const bool skip_subblock_edges );
};

class NormalLoopFilter
{
private:
  SimpleLoopFilter simple_;
  uint8_t high_edge_variance_threshold_;

  template <class BlockType>
  void filter_mb_vertical( BlockType & block );

  template <class BlockType>
  void filter_mb_horizontal( BlockType & block );

  template <class BlockType>
  void filter_sb_vertical( BlockType & block );

  template <class BlockType>
  void filter_sb_horizontal( BlockType & block );

public:
  NormalLoopFilter( const bool key_frame, const FilterParameters & params );

  void filter( Raster::Macroblock & raster, const bool skip_subblock_edges );
};

#endif /* LOOPFILTER_HH */
