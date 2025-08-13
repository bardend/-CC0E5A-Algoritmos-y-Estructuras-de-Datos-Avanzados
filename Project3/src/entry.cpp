#include "entry.h"
#include "config.h"

// Las implementaciones de entry están principalmente en el header ya que son templates
// Este archivo puede contener implementaciones específicas si es necesario

// Instanciaciones explícitas para los tipos que se usan
template class entry<MTreeConfig>;
template class metric_space<MTreeConfig>; 