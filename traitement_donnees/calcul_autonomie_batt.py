import matplotlib.pyplot as plt

consoA = 125
print(consoA)
consoE = 0.8+3.6*3+0.27

totalConso = 1000

durer_envoi = 20
print(durer_envoi)

taille_batterie = []
jours_batterie = []

for i in range(1,100):
    print(i*totalConso)
    taille_batterie.append(i*totalConso)
    nbparh = 1

    conso_A_h = ((durer_envoi*nbparh)/3600)*consoA 
    conso_E_h =  ((3600-(durer_envoi*nbparh))/3600)*consoE



    total_conso_h = conso_A_h + conso_E_h

    nb_h_batterie = (i*totalConso)/total_conso_h 
    nb_J_batterie =nb_h_batterie /24
    #print(total_conso_h)

    #print(nb_h_batterie)
    jours_batterie.append(nb_J_batterie)
    print(nb_J_batterie)
print(len(taille_batterie))
print(len(jours_batterie))


plt.plot(jours_batterie,taille_batterie)
plt.ylabel('Capacité de la batterie (mAh)')  # Inclure l'unité pour plus de clarté
plt.xlabel('Autonomie de la batterie (jours)')  # Plus explicite
plt.title("Relation entre la capacité de la batterie et son autonomie")
plt.legend()
plt.show()