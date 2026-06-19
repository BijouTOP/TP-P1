#include "reports.h"
#include "inventory.h"
#include "incidentes.h"
#include <stdio.h>
#include <time.h>

extern IncidentNode *getIncidentList(void);

void gerarRelatorioEstadoRede(void)
{
    FILE *file = fopen("data/relatorio_estado_rede.txt", "w");
    if (file == NULL)
        return;

    fprintf(file, "=========================================\n");
    fprintf(file, "       RELATÓRIO DE ESTADO DA REDE       \n");
    fprintf(file, "=========================================\n\n");

    fprintf(file, "--- EQUIPAMENTOS ---\n");
    Node *currentEquip = getInventoryList();
    int operacionais = 0, falhas = 0;

    while (currentEquip != NULL)
    {
        if (currentEquip->item.status == OPERACIONAL)
            operacionais++;
        if (currentEquip->item.status == FALHA)
            falhas++;
        currentEquip = currentEquip->next;
    }
    fprintf(file, "Equipamentos Operacionais: %d\n", operacionais);
    fprintf(file, "Equipamentos em Falha: %d\n\n", falhas);

    fprintf(file, "--- INCIDENTES PENDENTES ---\n");
    IncidentNode *currentIncident = getIncidentList();
    int pendentes = 0;

    while (currentIncident != NULL)
    {
        if (currentIncident->incident.status == INCIDENTE_PENDENTE)
        {
            fprintf(file, "ID: %d | Equipamento ID: %d | Tipo: %s | Descrição: %s\n",
                    currentIncident->incident.id,
                    currentIncident->incident.equipmentId,
                    currentIncident->incident.type,
                    currentIncident->incident.description);
            pendentes++;
        }
        currentIncident = currentIncident->next;
    }
    if (pendentes == 0)
        fprintf(file, "Nenhum incidente pendente.\n");

    fprintf(file, "\n--- LEITURAS ANÓMALAS DE SENSORES ---\n");
    fprintf(file, "Sem leituras anómalas registadas.\n");

    fclose(file);
}

void gerarRelatorioMensalIncidentes(void)
{
    FILE *file = fopen("data/relatorio_mensal_incidentes.txt", "w");
    if (file == NULL)
        return;

    fprintf(file, "=========================================\n");
    fprintf(file, "       RELATÓRIO MENSAL DE INCIDENTES    \n");
    fprintf(file, "=========================================\n\n");

    int totalBaixa = 0, totalMedia = 0, totalAlta = 0;

    IncidentNode *currentIncident = getIncidentList();

    while (currentIncident != NULL)
    {
        switch (currentIncident->incident.priority)
        {
        case PRIORIDADE_BAIXA:
            totalBaixa++;
            break;
        case PRIORIDADE_MEDIA:
            totalMedia++;
            break;
        case PRIORIDADE_ALTA:
            totalAlta++;
            break;
        }
        currentIncident = currentIncident->next;
    }

    fprintf(file, "--- TOTAIS POR PRIORIDADE ---\n");
    fprintf(file, "Prioridade Baixa: %d\n", totalBaixa);
    fprintf(file, "Prioridade Média: %d\n", totalMedia);
    fprintf(file, "Prioridade Alta:  %d\n\n", totalAlta);

    fclose(file);
}