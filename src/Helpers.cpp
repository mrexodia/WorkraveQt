#include "Helpers.h"

namespace Helpers
{
QString timeFormat(int seconds)
{
    auto minutes = seconds / 60;
    seconds = seconds % 60;
    return QString().sprintf("%d:%02d", minutes, seconds);
}
} // namespace Helpers
