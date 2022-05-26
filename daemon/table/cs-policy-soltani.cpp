

#include "cs-policy-soltani.hpp"
#include "cs.hpp"

namespace nfd {
namespace cs {
namespace soltani {

const std::string SoltaniPolicy::POLICY_NAME = "soltani";
NFD_REGISTER_CS_POLICY(SoltaniPolicy);

SoltaniPolicy::SoltaniPolicy()
  : Policy(POLICY_NAME)
{
}

void
SoltaniPolicy::doAfterInsert(EntryRef i)
{
  this->insertToQueue(i, true);
  this->evictEntries();
}

void
SoltaniPolicy::doAfterRefresh(EntryRef i)
{
  this->insertToQueue(i, false);
}

void
SoltaniPolicy::doBeforeErase(EntryRef i)
{
  m_queue.get<1>().erase(i);
}

void
SoltaniPolicy::doBeforeUse(EntryRef i)
{
  this->insertToQueue(i, false);
}

void
SoltaniPolicy::evictEntries()
{
  BOOST_ASSERT(this->getCs() != nullptr);
  while (this->getCs()->size() > this->getLimit()) {
    BOOST_ASSERT(!m_queue.empty());
    EntryRef i = m_queue.front();
    m_queue.pop_front();
    this->emitSignal(beforeEvict, i);
  }
}

void
SoltaniPolicy::insertToQueue(EntryRef i, bool isNewEntry)
{
  int visit;
  Queue::iterator it;
  bool isNew = false;
  // push_back only if i does not exist
  std::tie(it, isNew) = m_queue.push_back(i);

  BOOST_ASSERT(isNew == isNewEntry);
  if (!isNewEntry) {
    m_queue.relocate(m_queue.end(), it);
  }
}

} // namespace soltani
} // namespace cs
} // namespace nfd
