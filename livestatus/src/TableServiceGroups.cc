// Copyright (C) 2019 tribe29 GmbH - License: GNU General Public License v2
// This file is part of Checkmk (https://checkmk.com). It is subject to the
// terms and conditions defined in the file COPYING, which is part of this
// source code package.

#include "TableServiceGroups.h"

#include <memory>

#include "Column.h"
#include "OffsetStringColumn.h"
#include "Query.h"
#include "ServiceGroupMembersColumn.h"
#include "ServiceListStateColumn.h"
#include "auth.h"
#include "nagios.h"

/* this might be a hack (accessing Nagios' internal structures.
Ethan: please help me here: how should this be code to be
portable? */
extern servicegroup *servicegroup_list;

TableServiceGroups::TableServiceGroups(MonitoringCore *mc) : Table(mc) {
    addColumns(this, "", -1);
}

std::string TableServiceGroups::name() const { return "servicegroups"; }

std::string TableServiceGroups::namePrefix() const { return "servicegroup_"; }

// static
void TableServiceGroups::addColumns(Table *table, const std::string &prefix,
                                    int indirect_offset) {
    table->addColumn(std::make_unique<OffsetStringColumn>(
        prefix + "name", "The name of the service group",
        Column::Offsets{indirect_offset, -1, -1,
                        DANGEROUS_OFFSETOF(servicegroup, group_name)}));
    table->addColumn(std::make_unique<OffsetStringColumn>(
        prefix + "alias", "An alias of the service group",
        Column::Offsets{indirect_offset, -1, -1,
                        DANGEROUS_OFFSETOF(servicegroup, alias)}));
    table->addColumn(std::make_unique<OffsetStringColumn>(
        prefix + "notes", "Optional additional notes about the service group",
        Column::Offsets{indirect_offset, -1, -1,
                        DANGEROUS_OFFSETOF(servicegroup, notes)}));
    table->addColumn(std::make_unique<OffsetStringColumn>(
        prefix + "notes_url",
        "An optional URL to further notes on the service group",
        Column::Offsets{indirect_offset, -1, -1,
                        DANGEROUS_OFFSETOF(servicegroup, notes_url)}));
    table->addColumn(std::make_unique<OffsetStringColumn>(
        prefix + "action_url",
        "An optional URL to custom notes or actions on the service group",
        Column::Offsets{indirect_offset, -1, -1,
                        DANGEROUS_OFFSETOF(servicegroup, action_url)}));
    table->addColumn(std::make_unique<ServiceGroupMembersColumn>(
        prefix + "members",
        "A list of all members of the service group as host/service pairs",
        Column::Offsets{indirect_offset, -1, -1,
                        DANGEROUS_OFFSETOF(servicegroup, members)},
        table->core(), false));
    table->addColumn(std::make_unique<ServiceGroupMembersColumn>(
        prefix + "members_with_state",
        "A list of all members of the service group with state and has_been_checked",
        Column::Offsets{
            indirect_offset,
            -1,
            -1,
            DANGEROUS_OFFSETOF(servicegroup, members),
        },
        table->core(), true));

    table->addColumn(std::make_unique<ServiceListStateColumn>(
        prefix + "worst_service_state",
        "The worst soft state of all of the groups services (OK <= WARN <= UNKNOWN <= CRIT)",
        Column::Offsets{indirect_offset, -1, -1,
                        DANGEROUS_OFFSETOF(servicegroup, members)},
        table->core(), ServiceListStateColumn::Type::worst_state));
    table->addColumn(std::make_unique<ServiceListStateColumn>(
        prefix + "num_services", "The total number of services in the group",
        Column::Offsets{indirect_offset, -1, -1,
                        DANGEROUS_OFFSETOF(servicegroup, members)},
        table->core(), ServiceListStateColumn::Type::num));
    table->addColumn(std::make_unique<ServiceListStateColumn>(
        prefix + "num_services_ok",
        "The number of services in the group that are OK",
        Column::Offsets{indirect_offset, -1, -1,
                        DANGEROUS_OFFSETOF(servicegroup, members)},
        table->core(), ServiceListStateColumn::Type::num_ok));
    table->addColumn(std::make_unique<ServiceListStateColumn>(
        prefix + "num_services_warn",
        "The number of services in the group that are WARN",
        Column::Offsets{indirect_offset, -1, -1,
                        DANGEROUS_OFFSETOF(servicegroup, members)},
        table->core(), ServiceListStateColumn::Type::num_warn));
    table->addColumn(std::make_unique<ServiceListStateColumn>(
        prefix + "num_services_crit",
        "The number of services in the group that are CRIT",
        Column::Offsets{indirect_offset, -1, -1,
                        DANGEROUS_OFFSETOF(servicegroup, members)},
        table->core(), ServiceListStateColumn::Type::num_crit));
    table->addColumn(std::make_unique<ServiceListStateColumn>(
        prefix + "num_services_unknown",
        "The number of services in the group that are UNKNOWN",
        Column::Offsets{indirect_offset, -1, -1,
                        DANGEROUS_OFFSETOF(servicegroup, members)},
        table->core(), ServiceListStateColumn::Type::num_unknown));
    table->addColumn(std::make_unique<ServiceListStateColumn>(
        prefix + "num_services_pending",
        "The number of services in the group that are PENDING",
        Column::Offsets{indirect_offset, -1, -1,
                        DANGEROUS_OFFSETOF(servicegroup, members)},
        table->core(), ServiceListStateColumn::Type::num_pending));
    table->addColumn(std::make_unique<ServiceListStateColumn>(
        prefix + "num_services_handled_problems",
        "The number of services in the group that have handled problems",
        Column::Offsets{indirect_offset, -1, -1,
                        DANGEROUS_OFFSETOF(servicegroup, members)},
        table->core(), ServiceListStateColumn::Type::num_handled_problems));
    table->addColumn(std::make_unique<ServiceListStateColumn>(
        prefix + "num_services_unhandled_problems",
        "The number of services in the group that have unhandled problems",
        Column::Offsets{indirect_offset, -1, -1,
                        DANGEROUS_OFFSETOF(servicegroup, members)},
        table->core(), ServiceListStateColumn::Type::num_unhandled_problems));
    table->addColumn(std::make_unique<ServiceListStateColumn>(
        prefix + "num_services_hard_ok",
        "The number of services in the group that are OK",
        Column::Offsets{indirect_offset, -1, -1,
                        DANGEROUS_OFFSETOF(servicegroup, members)},
        table->core(), ServiceListStateColumn::Type::num_hard_ok));
    table->addColumn(std::make_unique<ServiceListStateColumn>(
        prefix + "num_services_hard_warn",
        "The number of services in the group that are WARN",
        Column::Offsets{indirect_offset, -1, -1,
                        DANGEROUS_OFFSETOF(servicegroup, members)},
        table->core(), ServiceListStateColumn::Type::num_hard_warn));
    table->addColumn(std::make_unique<ServiceListStateColumn>(
        prefix + "num_services_hard_crit",
        "The number of services in the group that are CRIT",
        Column::Offsets{indirect_offset, -1, -1,
                        DANGEROUS_OFFSETOF(servicegroup, members)},
        table->core(), ServiceListStateColumn::Type::num_hard_crit));
    table->addColumn(std::make_unique<ServiceListStateColumn>(
        prefix + "num_services_hard_unknown",
        "The number of services in the group that are UNKNOWN",
        Column::Offsets{indirect_offset, -1, -1,
                        DANGEROUS_OFFSETOF(servicegroup, members)},
        table->core(), ServiceListStateColumn::Type::num_hard_unknown));
}

void TableServiceGroups::answerQuery(Query *query) {
    for (servicegroup *sg = servicegroup_list; sg != nullptr; sg = sg->next) {
        if (!query->processDataset(Row(sg))) {
            break;
        }
    }
}

Row TableServiceGroups::findObject(const std::string &objectspec) const {
    return Row(find_servicegroup(const_cast<char *>(objectspec.c_str())));
}

bool TableServiceGroups::isAuthorized(Row row, const contact *ctc) const {
    return is_authorized_for_service_group(core(), rowData<servicegroup>(row),
                                           ctc);
}
