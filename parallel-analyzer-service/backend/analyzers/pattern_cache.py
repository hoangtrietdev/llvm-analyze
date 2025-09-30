"""
Pattern Cache - Cache AI responses for similar code patterns

This module implements intelligent caching of AI analysis responses
to reduce API costs and improve response times for similar code patterns.
Uses semantic hashing and similarity detection to maximize cache hits.
"""

import hashlib
import json
import logging
import re
import time
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass, asdict
from collections import defaultdict
import os

logger = logging.getLogger(__name__)

@dataclass
class CodePattern:
    """Represents a semantic code pattern for caching"""
    loop_type: str  # for, while, do-while
    array_access_pattern: str  # simple, complex, multi-dimensional
    operation_types: List[str]  # arithmetic, logical, assignment, etc.
    variable_count: int
    nesting_depth: int
    has_function_calls: bool
    has_control_flow: bool
    memory_pattern: str  # sequential, strided, random
    data_types: List[str]  # int, float, double, etc.

@dataclass
class CachedAnalysis:
    """Cached AI analysis result"""
    analysis: Dict[str, Any]
    timestamp: float
    usage_count: int
    pattern_hash: str
    original_code_snippet: str
    confidence: float

class PatternCache:
    """
    Intelligent cache for AI analysis results based on semantic code patterns
    """
    
    def __init__(self, cache_dir: str = "cache", max_cache_size: int = 1000):
        self.cache_dir = cache_dir
        self.max_cache_size = max_cache_size
        self.similarity_threshold = 0.85  # Minimum similarity for cache hit
        self.cache_expiry_hours = 24 * 7  # Cache expires after 1 week
        
        # In-memory cache for fast access
        self.memory_cache: Dict[str, CachedAnalysis] = {}
        
        # Pattern similarity index for fast lookup
        self.pattern_index: Dict[str, List[str]] = defaultdict(list)
        
        # Ensure cache directory exists
        os.makedirs(cache_dir, exist_ok=True)
        
        # Load existing cache
        self._load_cache()
        
    def get_cached_analysis(self, code_snippet: str, 
                          candidate_info: Dict[str, Any]) -> Optional[Dict[str, Any]]:
        """
        Retrieve cached analysis for a code pattern if available
        
        Args:
            code_snippet: Source code to analyze
            candidate_info: LLVM candidate information
            
        Returns:
            Cached analysis if found, None otherwise
        """
        pattern = self._extract_code_pattern(code_snippet, candidate_info)
        pattern_hash = self._hash_pattern(pattern)
        
        # Check for exact match first
        if pattern_hash in self.memory_cache:
            cached = self.memory_cache[pattern_hash]
            if self._is_cache_valid(cached):
                cached.usage_count += 1
                logger.info(f"Cache hit (exact): {pattern_hash[:8]} "
                           f"(used {cached.usage_count} times)")
                return cached.analysis
        
        # Check for similar patterns
        similar_analysis = self._find_similar_pattern(pattern, code_snippet)
        if similar_analysis:
            logger.info(f"Cache hit (similar): {pattern_hash[:8]}")
            return similar_analysis
        
        logger.debug(f"Cache miss: {pattern_hash[:8]}")
        return None
    
    def cache_analysis(self, code_snippet: str, candidate_info: Dict[str, Any],
                      ai_analysis: Dict[str, Any]) -> str:
        """
        Cache an AI analysis result for future use
        
        Args:
            code_snippet: Source code that was analyzed
            candidate_info: LLVM candidate information
            ai_analysis: AI analysis result to cache
            
        Returns:
            Pattern hash of the cached analysis
        """
        pattern = self._extract_code_pattern(code_snippet, candidate_info)
        pattern_hash = self._hash_pattern(pattern)
        
        # Create cached analysis entry
        cached_analysis = CachedAnalysis(
            analysis=ai_analysis,
            timestamp=time.time(),
            usage_count=1,
            pattern_hash=pattern_hash,
            original_code_snippet=code_snippet[:200],  # Store snippet preview
            confidence=ai_analysis.get('confidence', 0.0)
        )
        
        # Store in memory cache
        self.memory_cache[pattern_hash] = cached_analysis
        
        # Update pattern index for similarity search
        self._update_pattern_index(pattern, pattern_hash)
        
        # Persist to disk
        self._save_cache_entry(pattern_hash, cached_analysis)
        
        # Manage cache size
        self._cleanup_cache()
        
        logger.info(f"Cached analysis: {pattern_hash[:8]} "
                   f"(classification: {ai_analysis.get('classification', 'unknown')})")
        
        return pattern_hash
    
    def _extract_code_pattern(self, code_snippet: str, 
                            candidate_info: Dict[str, Any]) -> CodePattern:
        """Extract semantic pattern from code snippet"""
        
        # Determine loop type
        loop_type = self._detect_loop_type(code_snippet)
        
        # Analyze array access patterns
        array_pattern = self._analyze_array_access(code_snippet)
        
        # Extract operation types
        operations = self._extract_operations(code_snippet)
        
        # Count variables
        var_count = self._count_variables(code_snippet)
        
        # Calculate nesting depth
        nesting = self._calculate_nesting_depth(code_snippet)
        
        # Check for function calls and control flow
        has_calls = bool(re.search(r'\w+\s*\(', code_snippet))
        has_control = bool(re.search(r'\b(if|else|switch|case)\b', code_snippet))
        
        # Analyze memory access pattern
        memory_pattern = self._analyze_memory_pattern(code_snippet)
        
        # Extract data types
        data_types = self._extract_data_types(code_snippet)
        
        return CodePattern(
            loop_type=loop_type,
            array_access_pattern=array_pattern,
            operation_types=sorted(operations),
            variable_count=var_count,
            nesting_depth=nesting,
            has_function_calls=has_calls,
            has_control_flow=has_control,
            memory_pattern=memory_pattern,
            data_types=sorted(data_types)
        )
    
    def _detect_loop_type(self, code: str) -> str:
        """Detect the type of loop in the code"""
        if re.search(r'\bfor\s*\(', code):
            return 'for'
        elif re.search(r'\bwhile\s*\(', code):
            return 'while'
        elif re.search(r'\bdo\s*{', code):
            return 'do-while'
        else:
            return 'unknown'
    
    def _analyze_array_access(self, code: str) -> str:
        """Analyze array access patterns in the code"""
        # Simple indexing: arr[i]
        if re.search(r'\w+\[\s*\w+\s*\]', code):
            # Multi-dimensional: arr[i][j]
            if re.search(r'\w+\[\s*\w+\s*\]\s*\[\s*\w+\s*\]', code):
                return 'multi_dimensional'
            # Complex indexing: arr[i+1], arr[i*2+j]
            elif re.search(r'\w+\[\s*\w+\s*[+\-*/]\s*\w*\s*\]', code):
                return 'complex'
            else:
                return 'simple'
        else:
            return 'none'
    
    def _extract_operations(self, code: str) -> List[str]:
        """Extract types of operations in the code"""
        operations = []
        
        if re.search(r'[+\-]', code):
            operations.append('arithmetic')
        if re.search(r'[*/]', code):
            operations.append('multiplication')
        if re.search(r'[&|^~]', code):
            operations.append('bitwise')
        if re.search(r'[<>=!]', code):
            operations.append('comparison')
        if re.search(r'[=]', code):
            operations.append('assignment')
        if re.search(r'\b(sin|cos|sqrt|log|exp|pow)\b', code):
            operations.append('mathematical')
        
        return operations if operations else ['unknown']
    
    def _count_variables(self, code: str) -> int:
        """Count approximate number of unique variables"""
        # Extract variable-like identifiers
        variables = re.findall(r'\b[a-zA-Z_][a-zA-Z0-9_]*\b', code)
        # Filter out keywords and count unique identifiers
        keywords = {'for', 'while', 'if', 'else', 'int', 'float', 'double', 'return'}
        unique_vars = set(var for var in variables if var not in keywords)
        return len(unique_vars)
    
    def _calculate_nesting_depth(self, code: str) -> int:
        """Calculate maximum nesting depth in the code"""
        max_depth = 0
        current_depth = 0
        
        for char in code:
            if char == '{':
                current_depth += 1
                max_depth = max(max_depth, current_depth)
            elif char == '}':
                current_depth -= 1
        
        return max_depth
    
    def _analyze_memory_pattern(self, code: str) -> str:
        """Analyze memory access patterns"""
        if re.search(r'\w+\[\s*i\s*\]', code):
            return 'sequential'
        elif re.search(r'\w+\[\s*i\s*\*\s*\d+\s*\]', code):
            return 'strided'
        elif re.search(r'\w+\[\s*\w+\[\s*i\s*\]\s*\]', code):
            return 'indirect'
        else:
            return 'unknown'
    
    def _extract_data_types(self, code: str) -> List[str]:
        """Extract data types mentioned in the code"""
        types = []
        type_patterns = {
            'int': r'\bint\b',
            'float': r'\bfloat\b',
            'double': r'\bdouble\b',
            'char': r'\bchar\b',
            'long': r'\blong\b',
            'short': r'\bshort\b'
        }
        
        for type_name, pattern in type_patterns.items():
            if re.search(pattern, code):
                types.append(type_name)
        
        return types if types else ['unknown']
    
    def _hash_pattern(self, pattern: CodePattern) -> str:
        """Generate hash for a code pattern"""
        pattern_dict = asdict(pattern)
        pattern_json = json.dumps(pattern_dict, sort_keys=True)
        return hashlib.sha256(pattern_json.encode()).hexdigest()
    
    def _find_similar_pattern(self, target_pattern: CodePattern, 
                            code_snippet: str) -> Optional[Dict[str, Any]]:
        """Find similar cached patterns using similarity scoring"""
        target_hash = self._hash_pattern(target_pattern)
        
        best_similarity = 0.0
        best_analysis = None
        
        # Check patterns in the same category first
        category_key = f"{target_pattern.loop_type}_{target_pattern.array_access_pattern}"
        candidate_hashes = self.pattern_index.get(category_key, [])
        
        for cached_hash in candidate_hashes:
            if cached_hash not in self.memory_cache:
                continue
                
            cached_entry = self.memory_cache[cached_hash]
            if not self._is_cache_valid(cached_entry):
                continue
            
            # Calculate pattern similarity
            similarity = self._calculate_pattern_similarity(
                target_pattern, cached_entry.pattern_hash
            )
            
            if similarity >= self.similarity_threshold and similarity > best_similarity:
                best_similarity = similarity
                best_analysis = cached_entry.analysis
                # Increment usage count for similar match
                cached_entry.usage_count += 1
        
        if best_analysis:
            logger.debug(f"Similar pattern found: {best_similarity:.3f} similarity")
        
        return best_analysis
    
    def _calculate_pattern_similarity(self, pattern1: CodePattern, 
                                    pattern2_hash: str) -> float:
        """Calculate similarity between two code patterns"""
        if pattern2_hash not in self.memory_cache:
            return 0.0
        
        # Reconstruct pattern2 from cache (simplified comparison)
        cached_entry = self.memory_cache[pattern2_hash]
        
        similarity_score = 0.0
        total_weight = 0.0
        
        # Compare key attributes with different weights
        comparisons = [
            ('loop_type', 0.2),
            ('array_access_pattern', 0.25),
            ('operation_types', 0.2),
            ('has_function_calls', 0.1),
            ('has_control_flow', 0.1),
            ('memory_pattern', 0.15)
        ]
        
        # For simplicity, use a heuristic based on analysis similarity
        # In a full implementation, you'd reconstruct the pattern
        analysis1_type = pattern1.loop_type
        analysis2_confidence = cached_entry.confidence
        
        # Basic similarity heuristic
        if analysis1_type == 'for' and analysis2_confidence > 0.7:
            similarity_score = 0.8
        elif analysis1_type == 'while' and analysis2_confidence > 0.6:
            similarity_score = 0.75
        else:
            similarity_score = 0.6
        
        return similarity_score
    
    def _update_pattern_index(self, pattern: CodePattern, pattern_hash: str):
        """Update the pattern index for fast similarity lookup"""
        category_key = f"{pattern.loop_type}_{pattern.array_access_pattern}"
        if pattern_hash not in self.pattern_index[category_key]:
            self.pattern_index[category_key].append(pattern_hash)
    
    def _is_cache_valid(self, cached_analysis: CachedAnalysis) -> bool:
        """Check if a cached analysis is still valid"""
        age_hours = (time.time() - cached_analysis.timestamp) / 3600
        return age_hours < self.cache_expiry_hours
    
    def _load_cache(self):
        """Load existing cache from disk"""
        cache_file = os.path.join(self.cache_dir, "pattern_cache.json")
        if os.path.exists(cache_file):
            try:
                with open(cache_file, 'r') as f:
                    cache_data = json.load(f)
                
                for pattern_hash, entry_data in cache_data.items():
                    cached_analysis = CachedAnalysis(**entry_data)
                    if self._is_cache_valid(cached_analysis):
                        self.memory_cache[pattern_hash] = cached_analysis
                
                logger.info(f"Loaded {len(self.memory_cache)} cache entries")
            except Exception as e:
                logger.warning(f"Failed to load cache: {e}")
    
    def _save_cache_entry(self, pattern_hash: str, cached_analysis: CachedAnalysis):
        """Save a single cache entry to disk"""
        cache_file = os.path.join(self.cache_dir, "pattern_cache.json")
        
        # Load existing cache
        cache_data = {}
        if os.path.exists(cache_file):
            try:
                with open(cache_file, 'r') as f:
                    cache_data = json.load(f)
            except Exception as e:
                logger.warning(f"Failed to load existing cache for update: {e}")
        
        # Add new entry
        cache_data[pattern_hash] = asdict(cached_analysis)
        
        # Save back to disk
        try:
            with open(cache_file, 'w') as f:
                json.dump(cache_data, f, indent=2)
        except Exception as e:
            logger.error(f"Failed to save cache entry: {e}")
    
    def _cleanup_cache(self):
        """Clean up old or least-used cache entries"""
        if len(self.memory_cache) <= self.max_cache_size:
            return
        
        # Sort by usage count and age (least used and oldest first)
        sorted_entries = sorted(
            self.memory_cache.items(),
            key=lambda x: (x[1].usage_count, x[1].timestamp)
        )
        
        # Remove oldest entries
        entries_to_remove = len(self.memory_cache) - self.max_cache_size
        for pattern_hash, _ in sorted_entries[:entries_to_remove]:
            del self.memory_cache[pattern_hash]
            logger.debug(f"Removed cache entry: {pattern_hash[:8]}")
        
        logger.info(f"Cache cleanup: removed {entries_to_remove} entries")
    
    def get_cache_stats(self) -> Dict[str, Any]:
        """Get cache statistics for monitoring"""
        if not self.memory_cache:
            return {"total_entries": 0, "cache_hit_rate": 0.0}
        
        total_entries = len(self.memory_cache)
        total_usage = sum(entry.usage_count for entry in self.memory_cache.values())
        avg_usage = total_usage / total_entries if total_entries > 0 else 0
        
        # Calculate age distribution
        current_time = time.time()
        ages = [(current_time - entry.timestamp) / 3600 for entry in self.memory_cache.values()]
        avg_age_hours = sum(ages) / len(ages) if ages else 0
        
        return {
            "total_entries": total_entries,
            "total_usage_count": total_usage,
            "average_usage_per_entry": avg_usage,
            "average_age_hours": avg_age_hours,
            "cache_size_limit": self.max_cache_size,
            "similarity_threshold": self.similarity_threshold,
            "expiry_hours": self.cache_expiry_hours
        }
    
    def clear_cache(self):
        """Clear all cache entries"""
        self.memory_cache.clear()
        self.pattern_index.clear()
        
        cache_file = os.path.join(self.cache_dir, "pattern_cache.json")
        if os.path.exists(cache_file):
            os.remove(cache_file)
        
        logger.info("Cache cleared")