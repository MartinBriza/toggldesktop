#include <gtest/gtest.h>

#include "misc/types.h"
#include "misc/memory.h"

namespace toggl {

static int modelCounter = 0;

class BaseModel {
public:
    BaseModel(ProtectedContainerBase *, uuid_t uuid, id_t id)
        : uuid_(uuid)
        , id_(id)
    {
        modelCounter++;
    }
    ~BaseModel() {
        modelCounter--;
    }
    std::string ModelName() {
        return "Test";
    }
    uuid_t GUID() {
        return uuid_;
    }
    id_t ID() {
        return id_;
    }
    uuid_t uuid_;
    id_t id_;
};
// to make the mock class work with the library implementation
typedef BaseModel TestModel;

TEST(ProtectedContainer, Init) {
    ProtectedContainer<TestModel> container { nullptr };
    ASSERT_EQ(container.size(), 0);
    ASSERT_EQ(container.begin(), container.end());
    ASSERT_EQ(container.cbegin(), container.cend());
    ASSERT_EQ(modelCounter, 0);
}

TEST(ProtectedContainer, EmptySearch) {
    ProtectedContainer<TestModel> container { nullptr };
    ASSERT_EQ(container.size(), 0);

    auto first = *container.begin();
    ASSERT_FALSE(first);
    ASSERT_EQ(modelCounter, 0);
}

TEST(ProtectedContainer, Insert) {
    ProtectedContainer<TestModel> container { nullptr };
    ASSERT_EQ(container.size(), 0);
    container.create(uuid_t("abc"), 123);
    ASSERT_EQ(container.size(), 1);

    auto first = *container.begin();
    ASSERT_EQ(first->GUID(), "abc");
    first = container[size_t(0)];
    ASSERT_EQ(first->GUID(), "abc");
    auto byUuid = container["abc"];
    ASSERT_EQ(byUuid->GUID(), "abc");
    auto byId = container.byId(123);
    ASSERT_EQ(byId->GUID(), "abc");
    ASSERT_EQ(modelCounter, 1);
}

TEST(ProtectedContainer, InsertAndWrongSearch) {
    ASSERT_EQ(modelCounter, 0);
    ProtectedContainer<TestModel> container { nullptr };
    ASSERT_EQ(container.size(), 0);
    container.create(uuid_t("abc"), 123);
    ASSERT_EQ(container.size(), 1);

    auto second = *(++container.begin());
    ASSERT_FALSE(second);
    second = container[size_t(1)];
    ASSERT_FALSE(second);
    auto byUuid = container["cde"];
    ASSERT_FALSE(byUuid);
    auto byId = container.byId(321);
    ASSERT_FALSE(byId);
    ASSERT_EQ(modelCounter, 1);
}

TEST(ProtectedContainer, InsertAndRemoveByUuid) {
    ASSERT_EQ(modelCounter, 0);
    ProtectedContainer<TestModel> container { nullptr };
    ASSERT_EQ(container.size(), 0);
    container.create(uuid_t("1"), 100);
    ASSERT_EQ(modelCounter, 1);
    ASSERT_EQ(container.size(), 1);
    container.remove("1");
    ASSERT_EQ(container.size(), 0);

    auto first = *container.begin();
    ASSERT_FALSE(first);
    first = container[size_t(0)];
    ASSERT_FALSE(first);
    auto byUuid = container[uuid_t("1")];
    ASSERT_FALSE(byUuid);
    auto byId = container.byId(100);
    ASSERT_FALSE(byId);
    ASSERT_EQ(modelCounter, 0);
}

TEST(ProtectedContainer, InsertAndRemoveByIterator) {
    ASSERT_EQ(modelCounter, 0);
    ProtectedContainer<TestModel> container { nullptr };
    ASSERT_EQ(container.size(), 0);
    container.create(uuid_t("1"), 100);
    ASSERT_EQ(container.size(), 1);
    ASSERT_EQ(modelCounter, 1);
    auto it = container.begin();
    ASSERT_NE(it, container.end());
    container.erase(it);
    ASSERT_EQ(it, container.end());
    ASSERT_EQ(container.size(), 0);
    ASSERT_EQ(modelCounter, 0);

    auto first = *container.begin();
    ASSERT_FALSE(first);
    first = container[size_t(0)];
    ASSERT_FALSE(first);
    auto byUuid = container[uuid_t("1")];
    ASSERT_FALSE(byUuid);
    auto byId = container.byId(100);
    ASSERT_FALSE(byId);
}

TEST(ProtectedContainer, InsertAndRemoveMultiple) {
    ASSERT_EQ(modelCounter, 0);
    ProtectedContainer<TestModel> container { nullptr };
    ASSERT_EQ(container.size(), 0);
    container.create(uuid_t("1"), 100);
    ASSERT_EQ(container.size(), 1);
    container.create(uuid_t("2"), 200);
    ASSERT_EQ(container.size(), 2);
    container.create(uuid_t("3"), 300);
    ASSERT_EQ(container.size(), 3);
    container.create(uuid_t("4"), 400);
    ASSERT_EQ(container.size(), 4);
    container.create(uuid_t("5"), 500);
    ASSERT_EQ(container.size(), 5);
    ASSERT_EQ(modelCounter, 5);

    container.remove("3");
    ASSERT_EQ(container.size(), 4);
    auto item = container[uuid_t("3")];
    ASSERT_FALSE(item);
    item = container[uuid_t("1")];
    ASSERT_TRUE(item);
    item = container[uuid_t("2")];
    ASSERT_TRUE(item);
    item = container[uuid_t("4")];
    ASSERT_TRUE(item);
    item = container[uuid_t("5")];
    ASSERT_TRUE(item);
    container.remove("1");
    ASSERT_EQ(container.size(), 3);
    container.remove("5");
    ASSERT_EQ(container.size(), 2);
    container.remove("4");
    ASSERT_EQ(container.size(), 1);
    container.remove("2");
    ASSERT_EQ(container.size(), 0);
    ASSERT_EQ(modelCounter, 0);
}

TEST(ProtectedContainer, InsertAndClearMultiple) {
    ASSERT_EQ(modelCounter, 0);
    ProtectedContainer<TestModel> container { nullptr };
    ASSERT_EQ(container.size(), 0);
    container.create(uuid_t("1"), 100);
    ASSERT_EQ(container.size(), 1);
    container.create(uuid_t("2"), 200);
    ASSERT_EQ(container.size(), 2);
    container.create(uuid_t("3"), 300);
    ASSERT_EQ(container.size(), 3);
    container.create(uuid_t("4"), 400);
    ASSERT_EQ(container.size(), 4);
    container.create(uuid_t("5"), 500);
    ASSERT_EQ(container.size(), 5);
    ASSERT_EQ(modelCounter, 5);
    container.clear();
    ASSERT_EQ(container.size(), 0);
    ASSERT_EQ(modelCounter, 0);
}

} // namespace toggl

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
