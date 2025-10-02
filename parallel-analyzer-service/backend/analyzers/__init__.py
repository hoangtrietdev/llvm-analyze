"""
Enhanced Analyzer module initialization

Enhanced components:
- HotspotAnalyzer: Focus on loops that actually matter
- ConfidenceAnalyzer: Skip low-confidence candidates with OpenMP validation
- PatternCache: Cache AI responses for similar patterns
- OpenMPValidator: Validate pragmas against official OpenMP specification
"""

from .hotspot_analyzer import HotspotAnalyzer, LoopHotspot
from .confidence_analyzer import ConfidenceAnalyzer, ConfidenceLevel
from .pattern_cache import PatternCache, CodePattern, CachedAnalysis

# OpenMP validator may not be available if examples aren't downloaded
try:
    from .openmp_validator import OpenMPSpecValidator, ValidationStatus
    __all__ = [
        'HotspotAnalyzer', 'LoopHotspot',
        'ConfidenceAnalyzer', 'ConfidenceLevel', 
        'PatternCache', 'CodePattern', 'CachedAnalysis',
        'OpenMPSpecValidator', 'ValidationStatus'
    ]
except ImportError:
    __all__ = [
        'HotspotAnalyzer', 'LoopHotspot',
        'ConfidenceAnalyzer', 'ConfidenceLevel', 
        'PatternCache', 'CodePattern', 'CachedAnalysis'
    ]