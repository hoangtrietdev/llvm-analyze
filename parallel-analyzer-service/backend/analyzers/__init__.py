"""
Enhanced Analyzer module initialization

New enhanced components:
- HotspotAnalyzer: Focus on loops that actually matter
- ConfidenceAnalyzer: Skip low-confidence candidates  
- PatternCache: Cache AI responses for similar patterns
"""

from .hotspot_analyzer import HotspotAnalyzer, LoopHotspot
from .confidence_analyzer import ConfidenceAnalyzer, ConfidenceLevel
from .pattern_cache import PatternCache, CodePattern, CachedAnalysis

__all__ = [
    'HotspotAnalyzer', 'LoopHotspot',
    'ConfidenceAnalyzer', 'ConfidenceLevel', 
    'PatternCache', 'CodePattern', 'CachedAnalysis'
]